import 'dart:io';

Future main() async {
  var server = await HttpServer.bind(
    "0.0.0.0",
    4040,
  );
  print('Listening on localhost:${server.port}');

  await for (HttpRequest request in server) {
    request.response.write('Hello, world!');
    await request.response.close();
  }
}
