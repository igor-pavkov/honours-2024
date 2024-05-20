# from https://medium.com/@snyksec/code-injection-in-python-examples-and-prevention-680b44f3d9ae

user_input = input("Enter expression: ")
result = eval(user_input)  # Unsafe

