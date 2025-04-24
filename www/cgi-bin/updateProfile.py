#!/usr/bin/env python3

import os
import sys
import cgi

form = cgi.FieldStorage()

firstname = form.getvalue("firstname")
lastname = form.getvalue("lastname")
school = form.getvalue("school")
photo_item = form["photo"] if "photo" in form else None
photo_path = None

# Save profile info to a file
with open("../user_info.txt", "w") as f:
    f.write(f"First Name: {firstname}\n")
    f.write(f"Last Name: {lastname}\n")
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
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Profile Updated</title>
</head>
<body>
    <h1>Profile Updated</h1>
    <p><strong>First Name:</strong> {firstname}</p>
    <p><strong>Last Name:</strong> {lastname}</p>
    <p><strong>School:</strong> {school}</p>
""")

if photo_path:
    # You may need to adjust the image src based on your server's public path
    print(f'<p><strong>Profile Picture:</strong><br><img src="/uploads/{filename}" alt="Profile Picture" style="max-width:200px;"></p>')
