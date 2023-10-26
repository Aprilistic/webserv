#!/usr/bin/env python3
import cgi
import os
import sys

def handle_get(file_path):
    try:
        with open(file_path, 'r') as file:
            return file.read(), "200 OK"
    except FileNotFoundError:
        return "File not found", "404 Not Found"
    except Exception as e:
        return str(e), "500 Internal Server Error"

def handle_post_put(file_path, form, overwrite=False):
    try:
        mode = 'w' if overwrite else 'a'
        with open(file_path, mode) as file:
            for field in form.keys():
                file.write(f"{field}: {form.getvalue(field)}\n")
        return "Success", "200 OK"
    except Exception as e:
        return str(e), "500 Internal Server Error"

def handle_delete(file_path):
    try:
        os.remove(file_path)
        return "File deleted", "200 OK"
    except FileNotFoundError:
        return "File not found", "404 Not Found"
    except Exception as e:
        return str(e), "500 Internal Server Error"

def main():
    form = cgi.FieldStorage()
    file_path = form.getvalue('file', '')
    
    if not file_path:
        response_body = "File parameter is missing"
        status = "400 Bad Request"
    else:
        request_method = os.environ.get('REQUEST_METHOD', 'GET')
        if request_method == 'GET':
            response_body, status = handle_get(file_path)
        elif request_method in ['POST', 'PUT']:
            overwrite = (request_method == 'PUT')
            response_body, status = handle_post_put(file_path, form, overwrite)
        elif request_method == 'DELETE':
            response_body, status = handle_delete(file_path)
        else:
            response_body = "Method Not Allowed"
            status = "405 Method Not Allowed"

    print("Status:", status)
    print("Content-Type: text/plain")
    print(f"Content-Length: {len(response_body)}")
    print()
    sys.stdout.write(response_body)

if __name__ == "__main__":
    main()
