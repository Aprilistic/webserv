#!/usr/bin/env python

import os
from http.cookies import SimpleCookie

# HTTP header for content type, specifying HTML
print("Content-Type: text/html\n")

# HTML content with inline CSS for minimalistic design
print("""
<!DOCTYPE html>
<html>
<head>
    <title>Minimalistic Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            color: #333;
            background-color: #f8f8f8;
            margin: 40px;
            text-align: center;
        }
        .container {
            background-color: white;
            border: 1px solid #ddd;
            padding: 20px;
            display: inline-block;
        }
        h1 {
            color: #5a5a5a;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Cookie Information</h1>
""")

# Process the cookie
cookie_header = os.environ.get('HTTP_COOKIE')
if cookie_header:
    cookie = SimpleCookie()
    cookie.load(cookie_header)

    if 'id' in cookie:
        print("<p>Value of id cookie: {}</p>".format(cookie['id'].value))
    else:
        print("<p>Cookie 'id' not found in the cookie header.</p>")
else:
    print("<p>No cookie header found in environment variables.</p>")

# Close HTML tags
print("""
    </div>
</body>
</html>
""")

