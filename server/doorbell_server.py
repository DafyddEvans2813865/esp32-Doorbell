from flask import Flask, request
from datetime import datetime
import os

app = Flask(__name__)

SAVE_DIR = "server/captures"
os.makedirs(SAVE_DIR, exist_ok=True)

@app.route("/upload", methods=["POST"])
def upload():
    # raw JPEG bytes from ESP32
    img_data = request.data

    if not img_data:
        return "No data", 400

    # timestamp filename
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    filename = f"{SAVE_DIR}/img_{timestamp}.jpg"

    # save file
    with open(filename, "wb") as f:
        f.write(img_data)

    print(f"[DOORBELL] Saved: {filename} ({len(img_data)} bytes)")
    return "OK", 200


if __name__ == "__main__":
    print("Doorbell server running on http://0.0.0.0:5080")
    app.run(host="0.0.0.0", port=5080, debug=False)