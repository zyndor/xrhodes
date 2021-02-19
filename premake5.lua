--
-- XRhodes
--
-- copyright (c) Gyorgy Straub. All rights reserved.
--
-- License: https://github.com/zyndor/xrhodes#license-bsd-2-clause
--
--==============================================================================
include("template/workspace.lua")

do_workspace("xr3", "examples")

function do_vs_postbuild(dependencies)
  local artifacts_rel_path = "../../"..bin_location.."/"; -- we're currently in .projects/${platform}/
  for _, p in ipairs(tbl_platforms) do
    for _, c in ipairs(tbl_configurations) do
      filter { "platforms:"..p, c }
      local pc = p.."-"..c
      for _, iDep in ipairs(dependencies) do
        postbuildcommands{
          os.translateCommands("{COPY} $(VcpkgCurrentInstalledDir)"..
            "bin/"..iDep..".dll "..artifacts_rel_path..pc)
        }
      end
    end
  end
end

--
-- create a project that just reruns the premake script.
--
filter {}
project "!regenerate_projects"
kind "Utility"
local generate_projects_suffix = "";
if target_env == "windows" then
	generate_projects_suffix = ".bat"
else
	generate_projects_suffix = ".sh"
end

postbuildcommands{
	-- at this point we're in .projects/${target_env}/
	"../../tools/"..target_env.."/generate_projects"..generate_projects_suffix,
}

-- create projects
filter {}
include "xr3core/premake5.lua"

filter {}
include "xr3json/premake5.lua"

filter {}
include "xr3/premake5.lua"

filter {}
include "xr3scene/premake5.lua"

filter {}
include "unittests/premake5.lua"

filter {}
include "examples/premake5.lua"

