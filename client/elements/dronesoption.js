
let updop
const dronedispo = (orderid)=>orderid !== -1 && data.orders[orderid].status != 4
mn.insert(document.currentScript, (updater, oldupdater) => {
    
    if (!data.orders || !data.drones) {
        updop = updater
        mn.data.bind("orders", updop)
        mn.data.bind("drones", updop)
    } else {
        mn.data.remove_bind("orders", updop);
        mn.data.remove_bind("drones", updop);
    }

    mn.data.bind("orders", oldupdater(els=>{
        data.drones.forEach((drone, i) => {
            const d = els[i];
            const orderid = data.orders.findIndex((order)=>order.droneName === drone.name)
            d.toggleAttribute("disabled",  dronedispo(orderid))
        })
    }))

    if (!data.orders || !data.drones) return []
    return data.drones.map((drone, i) => {
        const d = mn.element.create(
            "option",
            {
                value: drone.name,
            },
            drone.name
        )
        const orderid = data.orders.findIndex((order)=>order.droneName === drone.name)
        d.toggleAttribute("disabled", dronedispo(orderid))
        return d
    })
})