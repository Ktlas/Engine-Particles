
-- this script is in lua format
-- this is a meta script that fills in data that is passed to 'myScript' that in turn calls C++ function
-- some of the data can be set by default, some of the data might be required to be edited from maya
function fillMetaInfoTable(args) -- the script fromat requires existance of this function

-- user modifed data
args['myScript']="StaticMesh.lua"
args['myScriptPackage']="Default"

-- static mesh
args['meshName'] = "imrod.x_imrodmesh_mesh.mesha"
args['meshPackage'] = "Default"

-- ImrdoEnemy info
args['health'] = 50
args['weapon'] = 'gun'
args['lethality'] = 50
args['range'] = 1000
args['behavior'] = 'random_patrol'

end -- required
