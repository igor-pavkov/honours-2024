TEST("Test exceptions")
{
    // ending with '%' character
    test_urldecode("Hello%20World%21%40%", "Hello World!@%");

    // incomplete ending with '%2'
    test_urldecode("Hello%20World%21%40%2", "Hello World!@%2");

    // improper encoding - '%%'
    test_urldecode("Hello%20World%21%40%%", "Hello World!@%%");

    // improper encoding - '%%%'
    test_urldecode("Hello%20World%21%40%%%", "Hello World!@%%%");

    // improper encoding - '%%%%'
    test_urldecode("Hello%20World%21%40%%%%", "Hello World!@%%%%");

    // improper encoding - '%%%61%%'
    test_urldecode("Hello%20World%21%40%%%61%%", "Hello World!@%%a%%");

    // non hexadecimal digits - '%1Q'
    test_urldecode("Hello%20World%21%40%1Q", "Hello World!@%1Q");
}