def find_user(db):
    id = input("Input user ID: ")
    SQL = f"SELECT * FROM Users WHERE UserId = {id}"
    db.Execute(SQL)