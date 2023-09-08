

-- expected arguments:

outputDebugString("Executing WayPoint.lua (note: this will probably just define functions like runScript(args)\n")

function runScript(args)
	outputDebugString("PE: Progress: WayPoint.lua runScript() Entry...\n")

    local pos = args['base']['pos']
    local u = args['base']['u']
    local v = args['base']['v']
    local n = args['base']['n']
    
    local needToRunToThisWaypoint = 0
    local randomizeNextWaypoint = 0
    if args['needToRunToThisWaypoint'] ~= nil then
        needToRunToThisWaypoint = args['needToRunToThisWaypoint']
    end
    if args['randomizeNextWaypoint'] ~= nil then
        randomizeNextWaypoint = args['randomizeNextWaypoint']
    end
    
    outputDebugString("PE: Progress: about to call root.CharacterControl.Events.Event_CREATE_WAYPOINT.Construct\n")

    evt = root.CharacterControl.Events.Event_CREATE_WAYPOINT.Construct(
        args['name'],
        args['next'],
        args['optionalNext1'],
        args['optionalNext2'],
        needToRunToThisWaypoint,
        randomizeNextWaypoint,
        pos[1], pos[2], pos[3],
        u[1], u[2], u[3],
        v[1], v[2], v[3],
        n[1], n[2], n[3],
        args['peuuid']
    )
    outputDebugString("PE: Progress: about to call getGameObjectManagerHandle()\n")

    handler = getGameObjectManagerHandle(l_getGameContext())
    outputDebugString("PE: Progress: about to call root.PE.Components.Component.SendEventToHandle(handler, evt)\n")

	root.PE.Components.Component.SendEventToHandle(handler, evt)
end
