#!/usr/bin/env python3

import cgi
import os

print("Content-Type: text/html\n")

form = cgi.FieldStorage()

name = form.getvalue("name", "Unknown")
age = form.getvalue("age", "Unknown")
school = form.getvalue("school", "Unknown")

# Save profile info to a file
with open("../user_info.txt", "w") as f:
    f.write(f"Name: {name}\n")
    f.write(f"Age: {age}\n")
    f.write(f"School: {school}\n")

# Handle uploaded photo
if "photo" in form:
    fileitem = form["photo"]
    if fileitem.filename:
        # Save the uploaded file as pp.jpg
        filepath = "../pp.jpg"
        with open(filepath, 'wb') as f:
            f.write(fileitem.file.read())

print(f"""
<html>
    <body>
        <h2>Profile updated successfully!</h2>
        <p><strong>Name:</strong> {name}</p>
        <p><strong>Age:</strong> {age}</p>
        <p><strong>School:</strong> {school}</p>
        <a href="/index.html">Back to Profile</a>
    </body>
</html>
""")
