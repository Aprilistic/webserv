#!/usr/bin/env python

import os
from http.cookies import SimpleCookie

# Set status code to 200 (OK)
status_code = "200 OK"

# Process the cookie
cookie_header = os.environ.get('HTTP_COOKIE')
if cookie_header:
    cookie = SimpleCookie()
    cookie.load(cookie_header)

    if 'id' not in cookie:
        status_code = "400 Bad Request"
else:
    status_code = "400 Bad Request"

# HTTP header for content type, specifying HTML
print("Status: {}".format(status_code))
print("Content-Type: text/html")

# HTML content with the updated structure
print("""
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Nuts Web Service</title>
    <style>
        body, html {
            height: 100vh;
            margin: 0;
            font-family: 'Gothic', sans-serif;
            background-color: rgba(21, 24, 23, 1);
            color: rgb(253, 253, 253);
            overflow-x: hidden;
            overflow-y: hidden;
        }
        *, *::before, *::after {
            box-sizing: border-box;
        }
        .top-nav {
            position: absolute;
            top: 0;
            width: 100%;
            display: flex;
            justify-content: space-between;
            padding: 25px 50px;
            border-bottom: 0.1px solid rgba(255, 255, 255, 0.2);
        }
        .nav-logo {
            font-size: 1.5em;
            color: #fff;
            text-decoration: none;
            font-weight: bold;
        }
        .nav-links a {
            color: #fff;
            font-weight: 100;
            text-decoration: none;
            margin-left: 30px;
        }
        .footer {
            position: absolute;
            bottom: 30px;
            width: 100%;
            padding: 10px;
            text-align: center;
            font-size: 0.8em;
        }
        .cookie-info {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            width: 50%;
            padding: 20px;
            border: 1px solid #fff;
            border-radius: 10px;
            text-align: center;
            background-color: rgba(255, 255, 255, 0.1);
        }
    </style>
</head>

<body>
    <header class="top-nav">
        <a href="./index.html" class="nav-logo">Nuts</a>
        <nav class="nav-links">
            <a href="./test.html">Updates</a>
            <a href="./about_nuts.html">About Nuts</a>
        </nav>
    </header>
    <div class="cookie-info">
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

# Close the cookie info div and the rest of the HTML tags
print("""
    </div>
    <div class="footer">
        <p>Powered by <strong>Nuts</strong></p>
    </div>
</body>

</html>
""")

# Save this script as cookieTest.py
