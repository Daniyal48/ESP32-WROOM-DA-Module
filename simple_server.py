from flask import Flask, request

app = Flask(__name__)

@app.route('/log', methods=['POST'])
def log():
	data = request.get_json()
	with open('logs.txt', 'a') as log_file:
		log_file.write(f"{data}\n")
	return "Log Received", 200

if __name__ == '__main__':
	app.run(host='0.0.0.0', port=5000)
  #This is a simple server will be updated with further functionalities
