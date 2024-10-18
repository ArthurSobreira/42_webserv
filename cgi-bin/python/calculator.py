#!/usr/bin/env python3
import cgi

form = cgi.FieldStorage()
num1 = float(form.getvalue("num1", 0))
num2 = float(form.getvalue("num2", 0))
operation = form.getvalue("operation", "add")

if operation == "add":
    result = num1 + num2
elif operation == "sub":
    result = num1 - num2
elif operation == "mul":
    result = num1 * num2
elif operation == "div" and num2 != 0:
    result = num1 / num2
else:
    result = "Invalid Operation"

print(f"<html><body><h1>Resultado: {result}</h1></body></html>")

# curl -X GET "http://localhost:4002/calculator?num1=10&num2=5&operation=div"