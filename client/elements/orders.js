
const STATUS = [
    "Drone en route vers le point de départ.", //0
    "Drone prêt à être envoyé. Cliquez pour valider.", //1
    "En cours de livraison.", //2
    "Livré. Cliquez pour confirmer la réception.", //3
    "Commande clôturée." //4
]

mn.insert(document.currentScript, (updater, oldupdater) => {

    mn.data.bind("orders", updater)
    console.log("UPDATE");
    // if (!data.orders) {
    // } else {
    // mn.data.remove_bind("orders", udporders);
    // }

    if (!data.orders) return []
    return data.orders.map((order) => {
        const bt = mn.element.create(
            "button",
            {
                class: "orderstatus",
                onclick: order.status === 1?
                ()=>{
                    req(`order/ready?id=${order.id}`)
                }:
                ()=>{
                    req(`order/recive?id=${order.id}`)
                }
            },
            STATUS[order.status]
        )
        bt.toggleAttribute("disabled", order.status !== 1 && order.status !== 3)
        return mn.element.create(
            "div",
            {
                class: 'order border',
                // onclick: () => {
                //     location.hash = ""
                //     location.hash = "#/order_" + order.id
                // }
            },
            mn.element.create(
                "div",
                {
                    class: "ordern",
                },
                "Order n°" + (order.id + 1)
            ),
            mn.element.create("div", { class: "vert_line" }),
            bt,
        )
    })
})