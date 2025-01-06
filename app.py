from flask import Flask, render_template, jsonify, request
import random  # For simulating sensor data

app = Flask(__name__)

# Simulated sensor data (replace with actual sensor readings)
def get_sensor_status():
    # Randomly simulate sensor data (status)
    status = random.choice(['healthy', 'lung_issue', 'diabetes', 'cancer'])
    return status

@app.route('/')
def index():
    # Get the sensor status
    status = get_sensor_status()
    return render_template('index.html', status=status)

@app.route('/status', methods=['GET'])
def status():
    # Send JSON response with sensor status
    status = get_sensor_status()
    return jsonify({"status": status})

if __name__ == "__main__":
    app.run(debug=True)
