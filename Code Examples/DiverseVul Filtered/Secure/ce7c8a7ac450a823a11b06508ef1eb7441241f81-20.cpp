TEST("Test urlencoded strings")
{
    test_urldecode("Hello%20World%21%40%23", "Hello World!@#");
    test_urldecode("Hello World!@#", "Hello World!@#");
    test_urldecode("%60%7E%21%40%23%24%25%5E%26%2A%28%29%2D%5F%3D%2B%5B%7B%5D%7D%5C%7C%3B%3A%27%22%2C%3C%2E%3E%2F%3F", \
    "`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?");
}