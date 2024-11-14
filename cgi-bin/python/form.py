#!/usr/bin/env python3

import cgi

def	main() -> None:
	form = cgi.FieldStorage()

	print("<html><body>")
	print("<h1>Python CGI Form</h1>")
	if "name" in form and "age" in form:
		name = form.getvalue("name")
		age = form.getvalue("age")
		print(f"<h2>Hello <u>{name}</u>, you are <u>{age}</u> years old!</h2>")
	else:
		print("<h2>Error: Missing name or age</h2>")

	print("</body></html>")

if __name__ == "__main__":
	main()

# curl -X GET "http://localhost:4000/form?name=John&age=30"
# curl -X POST -d "name=John&age=30" http://localhost:4000/form
