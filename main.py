from flask import Flask, request
from mqtt_server import send_mqtt

app = Flask(__name__)

@app.route('/api/<string:data_input>', methods=['GET', 'POST'])
def my_json(data_input):
    # print(data_input)
    if request.method == 'GET':
        send_mqtt(data_input)
        return 'success'
    return '200'

if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0', port=5000)