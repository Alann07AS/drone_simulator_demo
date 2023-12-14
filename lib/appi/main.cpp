#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <vector>
#include <array>
#include <tuple>
#include <Appi.h>
#include <Drone.h>

/* acount preset */
String const DEFAULT_PASS = "1234"; // default password
std::array<String, 3> const client1 = {"client1", DEFAULT_PASS, "d181ff0d-d205-4bed-ac54-73596121acaa"};
std::array<String, 3> const client2 = {"client2", DEFAULT_PASS, "cdc118b2-4c98-4b33-96e9-6e9e596a4ac5"};
std::array<String, 3> const admin = {"admin", DEFAULT_PASS, "dac9218f-8745-4f6b-b17e-010187a433fa"};

std::array<std::array<String, 3>, 3> const USERS = {client1, client2, admin};

bool isClientByName(String clientName)
{
    for (const std::array<String, 3> &user : USERS)
    {
        if (user[0 /*NAME*/] == clientName)
        {
            return true;
        }
    };
    return false;
}

// return UUID if credential is corect
String login(String pass, String login)
{
    for (const std::array<String, 3> &user : USERS)
    {
        if (user[0 /*NAME*/] == login && user[1 /*pass*/] == pass)
        {
            return user[2 /*UUID*/];
        }
    };
    return "";
};
/*_______________*/

/*ORDER PART*/
class Order
{
public:
    static std::vector<Order> orders; // all orders
    enum Status
    {
        NOT_READY,   // drone start point traveling
        READY,       // drone ready to start point
        IN_DELEVERY, // drone in delevery packet
        DELIVERED,   // drone arrived
        DONE,        // packet livred
    };
    enum Error
    {
        NONE,
        DRONE_UNVAIBLE,
        DRONE_NOT_FOUND,
        CLIENT_NOT_FOUND,
    };

    int id;
    Status status;
    String clientName;
    String droneName;
    float startLatitude;
    float startLongitude;
    float destLatitude;
    float destLongitude;

    // create new order with client name, drone name and start/end point coordones
    static Error newOrder(String clientName, String droneName, float startLatitude, float startLongitude, float destLatitude, float destLongitude)
    {
        const bool client = isClientByName(clientName);
        Drone *drone = Drone::getDroneByName(droneName);
        if (drone == NULL)
        {
            return Error::DRONE_NOT_FOUND;
        }
        if (!client)
        {
            return Error::CLIENT_NOT_FOUND;
        }
        if (drone->status != Drone::Status::LAND)
        {
            return Error::DRONE_UNVAIBLE;
        }

        // Register order
        orders.push_back(Order(
            clientName,
            droneName,
            startLatitude,
            startLongitude,
            destLatitude,
            destLongitude));
        Order *order = &orders[orders.size() - 1];

        // Send Drone to startpoint
        drone->setDest(startLatitude, startLongitude);

        // Invoke goToDest with your function or lambda
        drone->goToDest([order]()
                        { order->status = Order::Status::READY; });

        return Error::NONE;
    }; // generate order

    // valid packet ready to emmit if is possible(return true else false)
    bool emitPacket()
    {
        Drone *drone = Drone::getDroneByName(droneName);
        if (
            status != Status::READY ||
            drone == NULL)
        {
            return false;
        }
        drone->setDest(destLatitude, destLongitude);
        status = Status::IN_DELEVERY;
        drone->goToDest([this]()
                        { status = Status::DELIVERED; });
        return true;
    };
    // valid packet reception if is possible(return true else false)
    bool receivePacket()
    {
        Drone *drone = Drone::getDroneByName(droneName);
        if (
            status != Status::DELIVERED ||
            drone == NULL ||
            drone->land() // land if status DELIVRED into Check if landSucess
        )
        {
            return false;
        }
        status = Status::DONE;
        return true;
    };

    Order(String clientName, String droneName, float startLatitude, float startLongitude, float destLatitude, float destLongitude)
        : clientName(clientName), droneName(droneName), startLatitude(startLatitude),
          startLongitude(startLongitude), destLatitude(destLatitude), destLongitude(destLongitude), status(NOT_READY)
    {
        id = orders.size();
    }

    static Order *getOrderByUserNameAndId(const String &userName, int id)
    {
        for (Order &order : orders)
        {
            if (order.clientName == userName && order.id == id)
            {
                return &order;
            }
        }
        return NULL; // ou NULL dans le cas de C++
    }
};
std::vector<Order> Order::orders; // Define the static member

/*__________*/

void droneToJson(JsonObject &obj, const Drone &drone)
{
    obj["name"] = drone.name;
    obj["status"] = drone.status;
    obj["latitude"] = drone.latitude;
    obj["longitude"] = drone.longitude;
    obj["altitude"] = drone.altitude;
    obj["speed"] = drone.speed;
    obj["max_speed"] = drone.max_speed;
    obj["destLatitude"] = drone.destLatitude;
    obj["destLongitude"] = drone.destLongitude;
}

void orderToJson(JsonObject &obj, const Order &order)
{
    obj["id"] = order.id;
    obj["status"] = order.status;
    obj["clientName"] = order.clientName;
    obj["droneName"] = order.droneName;
    obj["startLatitude"] = order.startLatitude;
    obj["startLongitude"] = order.startLongitude;
    obj["destLatitude"] = order.destLatitude;
    obj["destLongitude"] = order.destLongitude;
}

/*____WEB_PART____*/

const String UUID = "UUID";
String getUserNameByUUID(AsyncWebServerRequest *request)
{
    // Get the request headers
    AsyncWebHeader *header = request->getHeader("Cookie");

    if (header)
    {
        printf("%s\n", header);
        // Parse the "Cookie" header to extract cookies
        String cookies = header->value();
        printf("%s\n", cookies);

        int uuidIndex = cookies.indexOf(UUID);

        if (uuidIndex != -1)
        {
            // The "UUID" cookie is present in the request
            int separatorIndex = cookies.indexOf('=', uuidIndex);
            int endIndex = cookies.indexOf(';', separatorIndex);
            if (endIndex == -1)
            {
                endIndex = cookies.length(); // If it's the last cookie in the list
            }

            String uuidValue = cookies.substring(separatorIndex + 1, endIndex);
            for (std::array<String, 3> client : USERS)
            {
                if (client[/*UUID*/ 2] == uuidValue)
                {
                    return client[0 /*NAME*/];
                }
            }
        }
    }

    // The "UUID" cookie is not present in the request
    return "";
}

// LOGIN
void handleLogin(AsyncWebServerRequest *request)
{
    // Extract login and pass from form data
    String log = request->arg("login");
    String pass = request->arg("pass");

    // Perform login
    const String uuid = login(pass, log);

    if (uuid == "")
    {
        // Login failed
        request->send(401, "text/plain", "Authentication failed");
    }
    else
    {
        // Login succeeded, set UUID cookie
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Login successful");
        response->addHeader("Set-Cookie", UUID + "=" + uuid);
        request->send(response);
    }
}

// ORDER
void handleOrder(AsyncWebServerRequest *request)
{
    const String userName = getUserNameByUUID(request);
    if (userName == "")
    {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    String droneName = request->arg("droneName");
    float startLatitude = request->arg("startLatitude").toFloat();
    float startLongitude = request->arg("startLongitude").toFloat();
    float destLatitude = request->arg("destLatitude").toFloat();
    float destLongitude = request->arg("destLongitude").toFloat();

    // The "UUID" cookie is present, continue with handling the order status
    const Order::Error error = Order::newOrder(
        userName,
        droneName,
        startLatitude,
        startLongitude,
        destLatitude,
        destLongitude);

    switch (error)
    {
    case Order::Error::NONE:
        request->send(200, "text/plain", "Order register suces");
        return;
    case Order::Error::CLIENT_NOT_FOUND:
        request->send(404, "text/plain", "Client not found");
        return;
    case Order::Error::DRONE_NOT_FOUND:
        request->send(404, "text/plain", "Drone not found");
        return;
    case Order::Error::DRONE_UNVAIBLE:
        request->send(403, "text/plain", "Drone unavailable");
        return;
    }
};

// GET ORDERS
void handleGetOrders(AsyncWebServerRequest *request)
{
    const String userName = getUserNameByUUID(request);
    if (userName == "")
    {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    DynamicJsonDocument doc(200); // doc(512)

    JsonArray ordersJa = doc.createNestedArray("orders");
    // Iterate through drones and add each one to the array
    for (const Order &order : Order::orders)
    {
        if (order.clientName == userName)
        {
            JsonObject orderJo = ordersJa.createNestedObject();
            orderToJson(orderJo, order);
        }
    }

    String jsonString;
    serializeJson(doc, jsonString);

    request->send(200, "application/json", jsonString);
};

// GET ORDER STATUS + DRONE INFO
void handleOrderStatus(AsyncWebServerRequest *request)
{
    const String userName = getUserNameByUUID(request);
    if (userName == "")
    {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    int orderId = request->arg("id").toInt();
    const Order *order = Order::getOrderByUserNameAndId(userName, orderId);
    const Drone *drone = order != NULL ? Drone::getDroneByName(order->droneName) : NULL;
    if (order == NULL || drone == NULL)
    {
        request->send(404, "text/plain", "Order or Drone NotFound");
        return;
    }

    DynamicJsonDocument doc(450); // doc(512)
    JsonObject clientJo = doc.createNestedObject("client");
    orderToJson(clientJo, *order);

    JsonObject droneJo = doc.createNestedObject("drone");
    droneToJson(droneJo, *drone);

    String jsonString;
    serializeJson(doc, jsonString);

    request->send(200, "application/json", jsonString);
};

// CONFIM READY
void handleSetOrderReady(AsyncWebServerRequest *request)
{
    const String userName = getUserNameByUUID(request);
    if (userName == "")
    {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    int orderId = request->arg("id").toInt();
    Serial.println("Et3");
    Order *order = Order::getOrderByUserNameAndId(userName, orderId);
    Serial.println("Et4");
    Drone *drone = Drone::getDroneByName(order->droneName);
    Serial.println("Et5");
    if (order == NULL || drone == NULL || !order->emitPacket())
    {
        request->send(404, "text/plain", "Order emit failed");
        return;
    }
    Serial.println("Et6");
    drone->setDest(order->destLatitude, order->destLongitude);
    drone->goToDest([order]()
                    { order->status = Order::Status::DONE; });
    request->send(200, "text/plain", "Order emit suces");
};

// CONFIM RECIVE
void handleSetOrderRecive(AsyncWebServerRequest *request)
{
    const String userName = getUserNameByUUID(request);
    if (userName == "")
    {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    int orderId = request->arg("id").toInt();
    Order *order = Order::getOrderByUserNameAndId(userName, orderId);
    if (order == NULL || !order->receivePacket())
    {
        request->send(404, "text/plain", "Order recive failed");
        return;
    }
    order->status = Order::Status::DONE;
    request->send(200, "text/plain", "Order recive suces");
};

// GET ALL DRONED INFO (pout le STYLE map de tout le TRAFIC)
void handleGetAllDroneInfo(AsyncWebServerRequest *request)
{
    const String userName = getUserNameByUUID(request);
    if (userName == "")
    {
        request->send(401, "text/plain", "Unauthorized");
        return;
    }
    DynamicJsonDocument doc(200); // doc(512)

    JsonArray dronesJa = doc.createNestedArray("drones");
    // Iterate through drones and add each one to the array
    for (const Drone &currentDrone : Drone::drones)
    {
        JsonObject droneJo = dronesJa.createNestedObject();
        droneToJson(droneJo, currentDrone);
    }

    String jsonString;
    serializeJson(doc, jsonString);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonString);

    // Set CORS headers
    response->addHeader("Access-Control-Allow-Origin", "http://127.0.0.1:5500");
    response->addHeader("Access-Control-Allow-Methods", "GET,OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type, Cookie");
    response->addHeader("Access-Control-Allow-Credentials", "true");

    request->send(response);
};

void initAppi(AsyncWebServer *server)
{

    server->on("/order", HTTP_POST, handleOrder);
    server->on("/orders", HTTP_GET, handleGetOrders);
    server->on("/order/ready", HTTP_GET, handleSetOrderReady);
    server->on("/order/recive", HTTP_GET, handleSetOrderRecive);
    server->on("/order/status", HTTP_GET, handleOrderStatus);
    server->on("/drones", HTTP_GET, handleGetAllDroneInfo);
    server->on("/login", HTTP_POST, handleLogin);
    server->onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Nothing");
    });

    server->begin();
};

/*________________*/
