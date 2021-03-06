/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/anakod/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 ****/

/** @defgroup   mqttclient MQTT client
 *  @brief      Provides MQTT client
 *  @ingroup    tcpclient
 *  @{
 */

#ifndef _SMING_CORE_NETWORK_MqttClient_H_
#define _SMING_CORE_NETWORK_MqttClient_H_

#define MQTT_MAX_BUFFER_SIZE 1024

class TcpClient;
#include "../Delegate.h"
#include "../../Wiring/WString.h"
#include "../../Wiring/WHashMap.h"
#include "libmosquitto/cpp/mosquittopp.h"
#include "URL.h"

typedef Delegate<void(String topic, String message)> MqttStringSubscriptionCallback;
typedef Delegate<void(uint16_t msgId, int type)> MqttMessageDeliveredCallback;
typedef Delegate<void(TcpClient& client, bool successful)> TcpClientCompleteDelegate;

class MqttClient;
class URL;

class MqttClient : public mosqpp::mosquittopp
{
public:
	MqttClient(bool autoDestruct = false);

	/** @brief  Construct an MQTT client 
	*  @deprecated Use instead the empty contructor
	*/
	MqttClient(String serverHost, int serverPort, MqttStringSubscriptionCallback callback = NULL);
	/** @brief  Construct an MQTT client
	*  @deprecated Use instead the empty contructor
	*/
	//MqttClient(IPAddress serverIp, int serverPort, MqttStringSubscriptionCallback callback = NULL);
	virtual ~MqttClient();

	/** @brief  Provide a funcion to be called when a message is received from the broker
	*/
	void setCallback(MqttStringSubscriptionCallback subscriptionCallback = NULL);
	
	void setCompleteDelegate(TcpClientCompleteDelegate completeCb);

	void setKeepAlive(int seconds);		 //send to broker
	void setPingRepeatTime(int seconds); //used by client
	// Sets Last Will and Testament
	bool setWill(const String& topic, const String& message, int QoS, bool retained = false);

	/** @brief  Connect to a MQTT server
	*  @param  url, in the form "mqtt://user:password@server:port" or "mqtts://user:password@server:port"
	*  @param  client name
	*/
	bool connect(const URL& url, const String& uniqueClientName, uint32_t sslOptions = 0);

	/** @brief  connect
	*  @deprecated Use connect(const String& url, const String& uniqueClientName) instead
	*/
	bool connect(const String& clientName, bool useSsl = false, uint32_t sslOptions = 0);
	/** @brief  connect
	*  @deprecated Use connect(const String& url, const String& uniqueClientName) instead
	*/
	bool connect(const String& clientName, const String& username, const String& password, bool useSsl = false,
				 uint32_t sslOptions = 0);

	bool publish(String topic, String message, bool retained = false);
	bool publishWithQoS(String topic, String message, int QoS, bool retained = false,
						MqttMessageDeliveredCallback onDelivery = NULL);

	bool subscribe(const String& topic);
	bool unsubscribe(const String& topic);
	
	void on_message(const struct mosquitto_message* message);

protected:
	void debugPrintResponseType(int type, int len);
	static int staticSendPacket(void* userInfo, const void* buf, unsigned int count);

private:
	bool privateConnect(const String& clientName, const String& username, const String& password,
						bool useSsl = false, uint32_t sslOptions = 0);

	URL url;
	mosqpp::mosquittopp mqtt;
	int waitingSize;
	uint8_t buffer[MQTT_MAX_BUFFER_SIZE + 1];
	uint8_t* current;
	int posHeader;
	MqttStringSubscriptionCallback callback;
	TcpClientCompleteDelegate completed = nullptr;
	int keepAlive = 60;
	int pingRepeatTime = 20;
	unsigned long lastMessage = 0;
	HashMap<uint16_t, MqttMessageDeliveredCallback> onDeliveryQueue;
};

/** @} */
#endif /* _SMING_CORE_NETWORK_MqttClient_H_ */
