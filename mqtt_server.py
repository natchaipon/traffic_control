import paho.mqtt.publish as publish
import json

hostname = "103.86.49.233"
port = 1883
auth = {
    'username': 'mymqtt',
    'password': '0932398390'
    }
def send_mqtt(value_send):
    value = value_send
    publish.single("server/send_mqtt", value, hostname=hostname, port=port, auth=auth)


# data = json.dumps({
#             'text': 'Hello, AdaBrain',
#             'user': 'It\'s me Ada'
#         })
# send_mqtt(data)