#!/usr/bin/env python3
import cgi

form = cgi.FieldStorage()

print("<html><body>")
if "name" in form and "age" in form:
	name = form.getvalue("name")
	age = form.getvalue("age")
	print(f"<h1>Hello {name}, you are {age} years old!</h1>")
else:
	print("<h1>Error: Missing name or age</h1>")

print("</body></html>")
