#!/usr/bin/env python3

import os
import cgi
import cgitb

cgitb.enable()

def	main() -> None:
	form = cgi.FieldStorage()
 
	print("<html><body>")
	if 'file' in form:
		file_item = form['file']
		upload_path = os.getenv('UPLOAD_PATH', '/tmp')

		if file_item.filename:
			filename = os.path.basename(file_item.filename)
			
			file_path = os.path.join(upload_path, filename)
			
			with open(file_path, 'wb') as output_file:
				output_file.write(file_item.file.read())

			print(f"<h1>File <u>{filename}</u> uploaded successfully to <u>{file_path}</u></h1>")
		else:
			print("<h1>No File Uploaded</h1></body>")
	else:
		print("<h1>No file field found in the form</h1></body>")
	print("</body></html>")

if __name__ == "__main__":
	main()

# curl -X POST -F "file=@/etc/passwd" http://localhost:4002/upload
