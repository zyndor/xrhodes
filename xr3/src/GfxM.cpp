//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GfxM.hpp"
#include "GfxCore.hpp"
#include "SyncJob.hpp"
#include "xr/threading/Worker.hpp"
#include "xr/threading/Spinlock.hpp"
#include "xr/memory/Pool.hpp"
#include "xr/memory/BufferReader.hpp"
#include "xr/events/SignalBroadcaster.hpp"
#include <cinttypes>

namespace xr
{
namespace Gfx
{
namespace
{

template <typename T>
T* Alloc(size_t size = 1)
{
  return static_cast<T*>(malloc(sizeof(T) * size));
}

uint8_t* CopyBuffer(Buffer const& buffer)
{
  uint8_t* bufferCopy = nullptr;
  if (buffer.size > 0)
  {
    bufferCopy = Alloc<uint8_t>(buffer.size);
    XR_ASSERTMSG(Gfx, bufferCopy, ("Failed to allocate %" PRIu32 " bytes", buffer.size));
    std::memcpy(bufferCopy, buffer.data, buffer.size);
  }
  return bufferCopy;
}

void ReleaseBuffer(uint8_t const** buffer)
{
  if (buffer)
  {
    free(const_cast<uint8_t*>(*buffer));
  }
}

template <typename T>
using Guard = std::unique_ptr<T, void(*)(T*)>;

using BufferGuard = Guard<uint8_t const*>;

//==============================================================================
enum class Command: uint8_t
{
  ReleaseVertexFormat,

  CreateVertexBuffer,
  ReleaseVertexBuffer,

  CreateIndexBuffer,
  ReleaseIndexBuffer,

  CreateInstanceDataBuffer,
  ReleaseInstanceDataBuffer,

  CreateTexture,
  ReleaseTexture,

  CreateFrameBufferWithPrivateTexture,
  CreateFrameBufferWithTextures,
  CreateFrameBufferWithAttachments,
  ReleaseFrameBuffer,

  ReleaseUniform,

  CreateShader,
  ReleaseShader,

  CreateProgram,
  ReleaseProgram,

  Clear,

  SetViewport,
  SetScissor,
  SetUniform,
  SetTexture,
  SetState,
  SetStencilState,
  SetInstanceData,
  SetProgram,
  SetFrameBuffer,

  Draw,
  DrawIndexed,
  Flush,
  Present,

  ReadFrameBuffer,

  End = 0xff
};

using CommandHeaderType = uint32_t;
static_assert(sizeof(CommandHeaderType) == 4, "Must be 4 bytes.");

CommandHeaderType FormatCommandHeader(Command cmd)
{
  return XR_FOURCC('c', 'm', 'd', cmd);
}

Command GetCommand(CommandHeaderType header)
{
  XR_ASSERT(Gfx, (header & 0xff) == 'c');
  XR_ASSERT(Gfx, ((header >> 8) & 0xff) == 'm');
  XR_ASSERT(Gfx, ((header >> 16) & 0xff) == 'd');
  return static_cast<Command>(header >> 24);
}

//==============================================================================
class MessageQueue
{
public: // structors
  MessageQueue()
  : mBuffer{ nullptr }
  {}

  ~MessageQueue()
  {
    Reset();
    free(mBuffer);
  }

public: // general
  void Init(size_t size)
  {
    uint8_t* buffer = static_cast<uint8_t*>(malloc(size));
    mBuffer = buffer;
    mPool.SetBuffer(size, false, buffer);
  }

  template <typename... Args>
  void WriteCommand(Command cmd, Args... args)
  {
    size_t size = sizeof(CommandHeaderType) + CountSize(args...);
    uint8_t*  chunk = static_cast<uint8_t*>(mPool.Allocate(size));
    if (chunk)
    {
      chunk = WriteCommandInternal(chunk, cmd);

      WriteParams(chunk, args...);
    }
    else
    {
      XR_TRACE(Gfx, ("Failed to allocate %" PRIu32 " bytes for a command %d.",
        cmd, size));
    }
  }

  void Reset()
  {
    mPool.Flush();
    mSemaphore.Post();
  }

  BufferReader GetReader() const
  {
    return BufferReader({ mPool.CalculateUsed(), mBuffer });
  }

  void Wait()
  {
    mSemaphore.Wait();
  }

private: // static
  static uint32_t CountSize()
  {
    return 0;
  }

  template <typename T, typename... Args>
  static uint32_t CountSize(T const& data, Args... args)
  {
    return sizeof(data) + CountSize(args...);
  }

  static uint8_t* WriteCommandInternal(uint8_t* buffer, Command cmd)
  {
    if (buffer)
    {
      *reinterpret_cast<CommandHeaderType*>(buffer) = FormatCommandHeader(cmd);
      buffer += sizeof(CommandHeaderType);
    }
    return buffer;
  }

  static void WriteParams(void*)
  {}

  template <typename T, typename... Args>
  static void WriteParams(void* buffer, T const& data, Args... args)
  {
    memcpy(buffer, &data, sizeof(data));
    WriteParams(reinterpret_cast<uint8_t*>(buffer) + sizeof(data), args...);
  }

private: // data
  Pool  mPool;
  uint8_t* mBuffer; // no ownership
  Semaphore mSemaphore;
};

//==============================================================================
struct CreateVertexBufferMessage
{
  VertexFormatHandle hFormat;
  Buffer buffer;
  FlagType flags;
  VertexBufferObject* vbo;
};

struct CreateIndexBufferMessage
{
  Buffer buffer;
  FlagType flags;
  IndexBufferObject* ibo;
};

struct CreateInstanceDataBufferMessage
{
  Buffer buffer;  // ownership
  InstanceDataStrideType stride;
  VertexBufferObject* idbo;
};

struct CreateTextureMessage
{
  Buffer* buffers;  // ownership or array and elements
  uint8_t numBuffers;
  TextureRef* texture;
};

struct CreateFrameBufferWithPrivateTextureMessage
{
  TextureFormat format;
  Px width;
  Px height;
  FlagType flags;
  FrameBufferObject* fbo;
};

struct CreateFrameBufferWithTexturesMessage
{
  uint8_t textureCount;
  TextureHandle* hTextures;  // ownership of array
  FrameBufferObject* fbo;
};

struct CreateFrameBufferWithAttachmentsMessage
{
  uint8_t attachmentCount;
  FrameBufferAttachment* attachments;  // ownership of array
  FrameBufferObject* fbo;
};

struct CreateShaderMessage
{
  ShaderType type;
  Buffer buffer;  // ownership
  ShaderRef* shader;
};

struct CreateProgramMessage
{
  ShaderHandle hVertex;
  ShaderHandle hFragment;
  Program* program;
};

struct ClearMessage
{
  FlagType flags;
  Color color;
  float depth;
  uint8_t stencil;
};

struct SetUniformMessage
{
  UniformHandle hUniform;
  Buffer buffer;
};

struct SetTextureMessage
{
  TextureHandle hTexture;
  uint8_t stage;
};

struct SetStencilStateMessage
{
  FlagType front;
  FlagType back;
};

struct SetInstanceDataMessage
{
  InstanceDataBufferHandle hIdbo;
  uint32_t offset;
  uint32_t count;
};

struct DrawMessage
{
  VertexBufferHandle hVbo;
  Primitive primitive;
  uint32_t offset;
  uint32_t count;
};

struct DrawIndexedMessage
{
  VertexBufferHandle hVbo;
  IndexBufferHandle hIbo;
  Primitive primitive;
  uint32_t offset;
  uint32_t count;
};

struct ReadFrameBufferMessage
{
  Px x;
  Px y;
  Px width;
  Px height;
  TextureFormat format;
  uint8_t iColorAttachment;
  void* buffer;
  ReadFrameBufferCompleteCallback* onComplete;
};

#define LOCK_RESOURCES std::unique_lock<Spinlock> lock(mResources->GetLock())

class RenderJob: public Worker::Job
{
public: // structors
  RenderJob(ResourceManager* resources, MessageQueue* messenger)
  : mResources(resources),
    mMessenger(messenger)
  {}

public: // general
  virtual bool Process() override
  {
    BufferReader reader = mMessenger->GetReader();

    CommandHeaderType commandHeader;
    // TODO: error communication
    bool more = true;
    while (more && reader.Read(commandHeader))
    {
      Command command = GetCommand(commandHeader);

#define COMMAND_CASE(x) case Command::x: x(reader); break;
      switch(command)
      {
      case Command::ReleaseVertexFormat:
        Release<VertexFormatHandle>(reader, Core::Release);
        break;

      COMMAND_CASE(CreateVertexBuffer)

      case Command::ReleaseVertexBuffer:
        Release<VertexBufferHandle>(reader, Core::Release);
        break;

      COMMAND_CASE(CreateIndexBuffer)

      case Command::ReleaseIndexBuffer:
        Release<IndexBufferHandle>(reader, Core::Release);
        break;

      COMMAND_CASE(CreateInstanceDataBuffer);

      case Command::ReleaseInstanceDataBuffer:
        Release<InstanceDataBufferHandle>(reader, Core::Release);
        break;

      COMMAND_CASE(CreateTexture)

      case Command::ReleaseTexture:
        Release<TextureHandle>(reader, Core::Release);
        break;

      COMMAND_CASE(CreateFrameBufferWithPrivateTexture)
      COMMAND_CASE(CreateFrameBufferWithTextures)
      COMMAND_CASE(CreateFrameBufferWithAttachments)

      case Command::ReleaseFrameBuffer:
        Release<FrameBufferHandle>(reader, Core::Release);
        break;

      COMMAND_CASE(ReleaseUniform)

      COMMAND_CASE(CreateShader)

      case Command::ReleaseShader:
        Release<ShaderHandle>(reader, Core::Release);
        break;

      COMMAND_CASE(CreateProgram)

      case Command::ReleaseProgram:
        Release<ProgramHandle>(reader, Core::Release);
        break;

      COMMAND_CASE(Clear)
      COMMAND_CASE(SetViewport)
      COMMAND_CASE(SetScissor)
      COMMAND_CASE(SetUniform)
      COMMAND_CASE(SetTexture)
      COMMAND_CASE(SetState)
      COMMAND_CASE(SetStencilState)
      COMMAND_CASE(SetInstanceData)
      COMMAND_CASE(SetProgram)
      COMMAND_CASE(SetFrameBuffer)
      COMMAND_CASE(Draw)
      COMMAND_CASE(DrawIndexed)

      case Command::Flush:
        Flush();
        break;

      COMMAND_CASE(Present)

      COMMAND_CASE(ReadFrameBuffer)

      case Command::End:
        more = false;
        break;
      }
#undef COMMAND_CASE
    }

    mMessenger->Reset();
    return true;
  }

private:// data
  ResourceManager* mResources;  // no ownership
  MessageQueue* mMessenger; // no ownership

private: // internal
  template <class T>
  void Release(BufferReader& reader, void(*releaseFn)(T))
  {
    T h;
    if (reader.Read(h))
    {
      LOCK_RESOURCES;
      releaseFn(h);
    }
  }

  void CreateVertexBuffer(BufferReader& reader)
  {
    CreateVertexBufferMessage m;
    BufferGuard bufferGuard(&m.buffer.data, ReleaseBuffer);
    if (reader.Read(m))
    {
      LOCK_RESOURCES;
      Core::CreateVertexBuffer(m.hFormat, m.buffer, m.flags, *m.vbo);
    }
  }

  void CreateIndexBuffer(BufferReader& reader)
  {
    CreateIndexBufferMessage m;
    BufferGuard bufferGuard(&m.buffer.data, ReleaseBuffer);
    if (reader.Read(m))
    {
      LOCK_RESOURCES;
      Core::CreateIndexBuffer(m.buffer, m.flags, *m.ibo);
    }
  }

  void CreateInstanceDataBuffer(BufferReader& reader)
  {
    CreateInstanceDataBufferMessage m;
    BufferGuard bufferGuard(&m.buffer.data, ReleaseBuffer);
    if (reader.Read(m))
    {
      LOCK_RESOURCES;
      Core::CreateInstanceDataBuffer(m.buffer, m.stride, *m.idbo);
    }
  }

  void CreateTexture(BufferReader& reader)
  {
    CreateTextureMessage m;
    m.buffers = nullptr;
    m.numBuffers = 0;
    Guard<CreateTextureMessage> guard(&m, [](CreateTextureMessage* mm){
      for (auto i = mm->buffers, iEnd = i + mm->numBuffers; i != iEnd; ++i)
      {
        ReleaseBuffer(&i->data);
      }
      free(mm->buffers);
    });

    if (reader.Read(m))
    {
      LOCK_RESOURCES;
      Core::CreateTexture(m.buffers, m.numBuffers, *m.texture);
    }
  }

  void CreateFrameBufferWithPrivateTexture(BufferReader& reader)
  {
    CreateFrameBufferWithPrivateTextureMessage m;
    if (reader.Read(m))
    {
      LOCK_RESOURCES;
      Core::CreateFrameBuffer(m.format, m.width, m.height, m.flags, *m.fbo);
    }
  }

  void CreateFrameBufferWithTextures(BufferReader& reader)
  {
    CreateFrameBufferWithTexturesMessage m;
    m.hTextures = nullptr;
    Guard<TextureHandle*> guard(&m.hTextures, [](TextureHandle** h) {
      free(*h);
    });
    if (reader.Read(m))
    {
      LOCK_RESOURCES;
      Core::CreateFrameBuffer(m.textureCount, m.hTextures, false, *m.fbo);
    }
  }

  void CreateFrameBufferWithAttachments(BufferReader& reader)
  {
    CreateFrameBufferWithAttachmentsMessage m;
    m.attachments = nullptr;
    Guard<FrameBufferAttachment*> guard(&m.attachments,
      [](FrameBufferAttachment** h) {
        free(*h);
      });
    if (reader.Read(m))
    {
      LOCK_RESOURCES;
      Core::CreateFrameBuffer(m.attachmentCount, m.attachments, false, *m.fbo);
    }
  }

  void ReleaseUniform(BufferReader& reader)
  {
    UniformHandle h;
    if (reader.Read(h))
    {
      LOCK_RESOURCES;
      mResources->Release(h);
    }
  }

  void CreateShader(BufferReader& reader)
  {
    CreateShaderMessage m;
    BufferGuard bufferGuard(&m.buffer.data, ReleaseBuffer);
    if (reader.Read(m))
    {
      LOCK_RESOURCES;
      Core::CreateShader(m.type, m.buffer, *m.shader);
    }
  }

  void CreateProgram(BufferReader& reader)
  {
    CreateProgramMessage m;
    if (reader.Read(m))
    {
      LOCK_RESOURCES;
      Core::CreateProgram(m.hVertex, m.hFragment, *m.program);
    }
  }

  void Clear(BufferReader& reader)
  {
    ClearMessage m;
    if (reader.Read(m))
    {
      Core::Clear(m.flags, m.color, m.depth, m.stencil);
    }
  }

  void SetViewport(BufferReader& reader)
  {
    Rect  rect;
    if (reader.Read(rect))
    {
      Core::SetViewport(rect);
    }
  }

  void SetScissor(BufferReader& reader)
  {
    Rect* rect = nullptr;
    Guard<Rect*> guard(&rect, [](Rect** rect_) {
      free(*rect_);
    });
    if (reader.Read(rect))
    {
      Core::SetScissor(rect);
    }
  }

  void SetUniform(BufferReader& reader)
  {
    SetUniformMessage m;
    BufferGuard guard(&m.buffer.data, ReleaseBuffer);
    if (reader.Read(m))
    {
      Core::SetUniform(m.hUniform, m.buffer);
    }
  }

  void SetTexture(BufferReader& reader)
  {
    SetTextureMessage m;
    if (reader.Read(m))
    {
      Core::SetTexture(m.hTexture, m.stage);
    }
  }

  void SetState(BufferReader& reader)
  {
    FlagType flags;
    if (reader.Read(flags))
    {
      Core::SetState(flags);
    }
  }

  void SetStencilState(BufferReader& reader)
  {
    SetStencilStateMessage m;
    if (reader.Read(m))
    {
      Core::SetStencilState(m.front, m.back);
    }
  }

  void SetInstanceData(BufferReader& reader)
  {
    SetInstanceDataMessage m;
    if (reader.Read(m))
    {
      Core::SetInstanceData(m.hIdbo, m.offset, m.count);
    }
  }

  void SetProgram(BufferReader& reader)
  {
    ProgramHandle h;
    if (reader.Read(h))
    {
      Core::SetProgram(h);
    }
  }

  void SetFrameBuffer(BufferReader& reader)
  {
    FrameBufferHandle h;
    if (reader.Read(h))
    {
      Core::SetFrameBuffer(h);
    }
  }

  void Draw(BufferReader& reader)
  {
    DrawMessage m;
    if (reader.Read(m))
    {
      Core::Draw(m.hVbo, m.primitive, m.offset, m.count);
    }
  }

  void DrawIndexed(BufferReader& reader)
  {
    DrawIndexedMessage m;
    if (reader.Read(m))
    {
      Core::Draw(m.hVbo, m.hIbo, m.primitive, m.offset, m.count);
    }
  }

  void Flush()
  {
    Core::Flush();
  }

  void Present(BufferReader& reader)
  {
    bool resetState;
    if (reader.Read(resetState))
    {
      Core::Present(resetState);
    }
  }

  void ReadFrameBuffer(BufferReader& reader)
  {
    ReadFrameBufferMessage m;
    m.onComplete = nullptr;
    Guard<ReadFrameBufferMessage> guard(&m, [](ReadFrameBufferMessage* mm) {
      delete mm->onComplete;
    });

    if (reader.Read(m))
    {
      Core::ReadFrameBuffer(m.x, m.y, m.width, m.height, m.format, m.iColorAttachment,
        m.buffer, m.onComplete);
    }
  }
};
#undef LOCK_RESOURCES

class MContext
{
public:// structors
  MContext(ResourceManager* resMan, uint32_t messageBufferSize)
  : mResources(resMan)
  {
    messageBufferSize = XR_ALIGN16(messageBufferSize);
    for (auto& q: mQueues)
    {
      q.Init(messageBufferSize);
    }
    mActiveQueue = mQueues;

    // Post to the second queue to indicate its immediate availability,
    // as there's nothing to wait on initially.
    mQueues[1].Reset();
  }

  ~MContext()
  {
    mRenderThread.Finalize();
  }

public: // general
  ResourceManager& GetResources()
  {
    return *mResources;
  }

  MessageQueue& GetActiveQueue()
  {
    return *mActiveQueue;
  }

  void EnqueueJob(Worker::Job& job)
  {
    mRenderThread.Enqueue(job);
  }

  void SubmitCommands()
  {
    mActiveQueue->WriteCommand(Command::End);
    auto renderJob = new RenderJob(mResources, mActiveQueue);
    mRenderThread.Enqueue(*renderJob);

    auto next = mQueues + ((mActiveQueue - mQueues + 1) & (XR_ARRAY_SIZE(mQueues) - 1));
    mActiveQueue = next;
    next->Wait();
  }

private: // data
  ResourceManager* mResources;
  MessageQueue mQueues[2];
  MessageQueue* mActiveQueue = nullptr;
  Worker mRenderThread;

  static_assert((XR_ARRAY_SIZE(MContext::mQueues) & (XR_ARRAY_SIZE(MContext::mQueues) - 1)) == 0, "Power of two");
};

std::unique_ptr<MContext> sContext;

} // nonamespace

//==============================================================================
void M::Init(Context* context, ResourceManager* resources, uint32_t messageBufferSize)
{
  XR_ASSERT(GfxM, !sContext);
  sContext.reset(new MContext(resources, messageBufferSize));

  SyncJob initJob{ [context, resources]() { Core::Init(context, resources); } };
  sContext->EnqueueJob(initJob);
}

//==============================================================================
VertexFormatHandle M::RegisterVertexFormat(VertexFormat const& format)
{
  std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
  return sContext->GetResources().RegisterVertexFormat(format);
}

//==============================================================================
void M::Release(VertexFormatHandle h)
{
  std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
  sContext->GetActiveQueue().WriteCommand(Command::ReleaseVertexFormat, h);
}

//==============================================================================
VertexBufferHandle M::CreateVertexBuffer(VertexFormatHandle hFormat,
  Buffer const& buffer, FlagType flags)
{
  VertexBufferHandle h;
  auto bufferCopy = CopyBuffer(buffer);
  auto& vbos = sContext->GetResources().GetVbos();
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    h.id = static_cast<uint16_t>(vbos.server.Acquire());
  };

  sContext->GetActiveQueue().WriteCommand(Command::CreateVertexBuffer,
    CreateVertexBufferMessage{ hFormat, Buffer{ buffer.size, bufferCopy },
      flags, vbos.data + h.id });
  return h;
}

//==============================================================================
void M::Release(VertexBufferHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::ReleaseVertexBuffer, h);
}

//==============================================================================
IndexBufferHandle M::CreateIndexBuffer(Buffer const& buffer, FlagType flags)
{
  IndexBufferHandle h;
  auto bufferCopy = CopyBuffer(buffer);
  auto& ibos = sContext->GetResources().GetIbos();
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    h.id = static_cast<uint16_t>(ibos.server.Acquire());
  };

  sContext->GetActiveQueue().WriteCommand(Command::CreateIndexBuffer,
    CreateIndexBufferMessage{ Buffer { buffer.size, bufferCopy }, flags,
      ibos.data + h.id });
  return h;
}

//==============================================================================
void M::Release(IndexBufferHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::ReleaseIndexBuffer, h);
}

//==============================================================================
InstanceDataBufferHandle M::CreateInstanceDataBuffer(Buffer const& buffer, InstanceDataStrideType stride)
{
  InstanceDataBufferHandle h;
  auto bufferCopy = CopyBuffer(buffer);
  auto& vbos = sContext->GetResources().GetVbos();
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    h.id = static_cast<uint16_t>(vbos.server.Acquire());
  }

  sContext->GetActiveQueue().WriteCommand(Command::CreateInstanceDataBuffer,
    CreateInstanceDataBufferMessage{ Buffer { buffer.size, bufferCopy }, stride,
      vbos.data + h.id });
  return h;
}

//==============================================================================
void M::Release(InstanceDataBufferHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::ReleaseInstanceDataBuffer, h);
}

//==============================================================================
TextureHandle M::CreateTexture(TextureFormat format, Px width,
  Px height, Px depth, FlagType flags, Buffer const* buffers,
  uint8_t numBuffers)
{
  auto buffersCopy = Alloc<Buffer>(numBuffers);
  if (!buffersCopy)
  {
    return TextureHandle();
  }

  struct BufferListGuard
  {
    Buffer* buffers;
    uint8_t numBuffers;

    ~BufferListGuard()
    {
      if (buffers)
      {
        for (auto i = buffers, iEnd = i + numBuffers; i != iEnd; ++i)
        {
          ReleaseBuffer(&i->data);
        }
        free(buffers);
      }
    }
  } buffersGuard { buffersCopy, numBuffers };

  for (size_t i = 0; i < numBuffers; ++i)
  {
    buffersCopy[i] = { buffers[i].size, CopyBuffer(buffers[i]) };
    if (buffers[i].size > 0 && !buffersCopy[i].data)
    {
      return TextureHandle();
    }
  }

  TextureHandle h;
  TextureRef* textureRef = nullptr;
  auto& textures = sContext->GetResources().GetTextures();
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    h.id = static_cast<uint16_t>(textures.server.Acquire());
    textureRef = textures.data + h.id;
    textureRef->inst.info = TextureInfo { format, width, height, depth,
      TextureInfo::CalculateMipLevels(width, height, flags), flags };
  }

  sContext->GetActiveQueue().WriteCommand(Command::CreateTexture,
    CreateTextureMessage { buffersCopy, numBuffers, textureRef });

  buffersGuard.buffers = nullptr; // dismiss guard
  return h;
}

//==============================================================================
TextureInfo M::GetTextureInfo(TextureHandle h)
{
  std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
  return sContext->GetResources().GetTextures().data[h.id].inst.info;
}

//==============================================================================
void M::Release(TextureHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::ReleaseTexture, h);
}

//==============================================================================
FrameBufferHandle M::CreateFrameBuffer(TextureFormat format, Px width,
  Px height, FlagType flags)
{
  FrameBufferHandle h;
  auto& fbos = sContext->GetResources().GetFbos();
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    h.id = static_cast<uint16_t>(fbos.server.Acquire());
  }

  sContext->GetActiveQueue().WriteCommand(Command::CreateFrameBufferWithPrivateTexture,
    CreateFrameBufferWithPrivateTextureMessage{ format, width, height, flags,
      fbos.data + h.id });
  return h;
}

//==============================================================================
FrameBufferHandle M::CreateFrameBuffer(uint8_t textureCount,
  TextureHandle const* hTextures)
{
  auto hTexturesCopy = Alloc<TextureHandle>(textureCount);
  if (!hTexturesCopy)
  {
    return FrameBufferHandle();
  }

  for (uint8_t i = 0; i < textureCount; ++i)
  {
    hTexturesCopy[i] = hTextures[i];
  }

  FrameBufferHandle h;
  auto& fbos = sContext->GetResources().GetFbos();
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    h.id = static_cast<uint16_t>(fbos.server.Acquire());
  }

  sContext->GetActiveQueue().WriteCommand(Command::CreateFrameBufferWithTextures,
    CreateFrameBufferWithTexturesMessage{ textureCount, hTexturesCopy, fbos.data + h.id });
  return h;
}

//==============================================================================
FrameBufferHandle M::CreateFrameBuffer(uint8_t attachmentCount,
  FrameBufferAttachment const* attachments)
{
  auto attachmentsCopy = Alloc<FrameBufferAttachment>(attachmentCount);
  if (!attachmentsCopy)
  {
    return FrameBufferHandle();
  }

  for (uint8_t i = 0; i < attachmentCount; ++i)
  {
    attachmentsCopy[i] = attachments[i];
  }

  FrameBufferHandle h;
  auto& fbos = sContext->GetResources().GetFbos();
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    h.id =  static_cast<uint16_t>(fbos.server.Acquire());
  }

  sContext->GetActiveQueue().WriteCommand(Command::CreateFrameBufferWithAttachments,
    CreateFrameBufferWithAttachmentsMessage{ attachmentCount, attachmentsCopy,
      fbos.data + h.id });
  return h;
}

//==============================================================================
void M::Release(FrameBufferHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::ReleaseFrameBuffer, h);
}

//==============================================================================
UniformHandle M::CreateUniform(char const* name, UniformType type, uint8_t arraySize)
{
  std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
  return sContext->GetResources().CreateUniform(name, type, arraySize);
}

//==============================================================================
void M::Release(UniformHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::ReleaseUniform, h);
}

//==============================================================================
ShaderHandle M::CreateShader(ShaderType type, Buffer const& buffer)
{
  auto bufferCopy = CopyBuffer(buffer);
  if (!bufferCopy)
  {
    return ShaderHandle();
  }

  ShaderHandle h;
  auto& shaders = sContext->GetResources().GetShaders();
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    h.id = static_cast<uint16_t>(shaders.server.Acquire());
  }

  sContext->GetActiveQueue().WriteCommand(Command::CreateShader,
    CreateShaderMessage{ type, Buffer{ buffer.size, bufferCopy }, shaders.data + h.id });
  return h;
}

//==============================================================================
void M::Release(ShaderHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::ReleaseShader, h);
}

//==============================================================================
ProgramHandle M::CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment)
{
  ProgramHandle h;
  auto& programs = sContext->GetResources().GetPrograms();
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    h.id = static_cast<uint16_t>(programs.server.Acquire());
  }

  sContext->GetActiveQueue().WriteCommand(Command::CreateProgram,
    CreateProgramMessage{ hVertex, hFragment, programs.data + h.id });
  return h;
}

//==============================================================================
void M::Release(ProgramHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::ReleaseProgram, h);
}

//==============================================================================
void M::Clear(FlagType flags, Color const& color, float depth, uint8_t stencil)
{
  sContext->GetActiveQueue().WriteCommand(Command::Clear,
    ClearMessage{ flags, color, depth, stencil });
}

//==============================================================================
void M::SetViewport(Rect const& rect)
{
  sContext->GetActiveQueue().WriteCommand(Command::SetViewport, rect);
}

//==============================================================================
void M::SetScissor(Rect const* rect)
{
  Rect* rectCopy = nullptr;
  if (rect)
  {
    rectCopy = Alloc<Rect>();
  }

  if (!rect || rectCopy)
  {
    sContext->GetActiveQueue().WriteCommand(Command::SetViewport, rectCopy);
  }
}

//==============================================================================
void M::SetUniform(UniformHandle h, uint8_t numElems, void const* data)
{
  size_t byteSize = 0;
  {
    std::unique_lock<Spinlock> lock(sContext->GetResources().GetLock());
    byteSize = sContext->GetResources().GetUniformDataSize(h, numElems);
  }
  Buffer buffer = { byteSize, CopyBuffer(Buffer{ byteSize,
    reinterpret_cast<uint8_t const*>(data) }) };
  sContext->GetActiveQueue().WriteCommand(Command::SetUniform,
    SetUniformMessage{ h, buffer });
}

//==============================================================================
void M::SetTexture(TextureHandle h, uint8_t stage)
{
  sContext->GetActiveQueue().WriteCommand(Command::SetTexture,
    SetTextureMessage{ h, stage });
}

//==============================================================================
void M::SetState(FlagType flags)
{
  sContext->GetActiveQueue().WriteCommand(Command::SetState, flags);
}

//==============================================================================
void M::SetStencilState(FlagType front, FlagType back)
{
  sContext->GetActiveQueue().WriteCommand(Command::SetStencilState,
    SetStencilStateMessage{ front, back });
}

//==============================================================================
void M::SetInstanceData(InstanceDataBufferHandle h, uint32_t offset, uint32_t count)
{
  sContext->GetActiveQueue().WriteCommand(Command::SetInstanceData,
    SetInstanceDataMessage{ h, offset, count });
}

//==============================================================================
void M::SetProgram(ProgramHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::SetProgram, h);
}

//==============================================================================
void M::SetFrameBuffer(FrameBufferHandle h)
{
  sContext->GetActiveQueue().WriteCommand(Command::SetFrameBuffer, h);
}

//==============================================================================
void M::Draw(VertexBufferHandle vbh, Primitive pt, uint32_t offset, uint32_t count)
{
  sContext->GetActiveQueue().WriteCommand(Command::Draw,
    DrawMessage{ vbh, pt, offset, count });
}

//==============================================================================
void M::Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive pt,
  uint32_t offset, uint32_t count)
{
  sContext->GetActiveQueue().WriteCommand(Command::DrawIndexed,
    DrawIndexedMessage{ vbh, ibh, pt, offset, count });
}

//==============================================================================
void M::Flush()
{
  sContext->GetActiveQueue().WriteCommand(Command::Flush);
  Core::OnFlush();

  sContext->SubmitCommands();
}

//==============================================================================
void M::Present(bool resetState)
{
  sContext->GetActiveQueue().WriteCommand(Command::Present, resetState);
  Core::OnFlush();

  sContext->SubmitCommands();
}

//==============================================================================
void M::ReadFrameBuffer(Px x, Px y, Px width, Px height,
  TextureFormat format, uint8_t colorAttachment, void* mem,
  ReadFrameBufferCompleteCallback* onComplete)
{
  sContext->GetActiveQueue().WriteCommand(Command::ReadFrameBuffer,
    ReadFrameBufferMessage{ x, y, width, height, format, colorAttachment, mem,
      onComplete });
  sContext->SubmitCommands();
}

//==============================================================================
Signal<void>& M::FlushSignal()
{
  return Core::FlushSignal();
}

//==============================================================================
Signal<void>& M::ShutdownSignal()
{
  return Core::ShutdownSignal();
}

//==============================================================================
void M::Shutdown()
{
  Core::OnShutdown();
  SyncJob shutdownJob{ []() { Core::Shutdown(); } };
  sContext->EnqueueJob(shutdownJob);
  sContext.reset();
}

} // Gfx
}
