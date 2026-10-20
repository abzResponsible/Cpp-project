#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

namespace {

const int kPort = 8080;

const char* kCss = R"CSS(
:root {
  --bg: #f3f1ec;
  --paper: #faf9f6;
  --ink: #1a1a1a;
  --muted: #3f3f3f;
  --line: #cfc9bb;
  --nav: #1f2428;
  --nav-text: #f4f1ea;
  --focus: #243447;
}
* { box-sizing: border-box; }
html, body { margin: 0; padding: 0; }
body {
  background: var(--bg);
  color: var(--ink);
  font-family: Georgia, "Times New Roman", Times, serif;
  font-size: 1.125rem;
  line-height: 1.7;
}
a { color: var(--focus); text-decoration: underline; text-underline-offset: 0.15em; }
a:hover { color: var(--ink); }
.skip {
  position: absolute; left: -999px;
}
.skip:focus { left: 1rem; top: 1rem; background: var(--paper); padding: 0.4rem 0.7rem; }
header {
  background: var(--nav);
  color: var(--nav-text);
  padding: 1rem 1.5rem;
}
header .wrap, main .wrap, footer .wrap {
  max-width: 44rem;
  margin: 0 auto;
}
.brand {
  font-size: 1.15rem;
  letter-spacing: 0.02em;
  margin: 0 0 0.6rem;
  font-weight: 700;
}
nav a {
  color: var(--nav-text);
  text-decoration: none;
  margin-right: 1.25rem;
  font-family: "Segoe UI", Tahoma, sans-serif;
  font-size: 1rem;
  border-bottom: 2px solid transparent;
}
nav a:hover, nav a[aria-current="page"] {
  border-bottom-color: #e6e1d4;
  color: #ffffff;
}
main { padding: 2.25rem 1.5rem 3rem; }
.card {
  background: var(--paper);
  border: 1px solid var(--line);
  padding: 1.75rem 1.6rem;
}
h1 {
  font-size: 2.05rem;
  line-height: 1.25;
  margin: 0 0 0.85rem;
}
h2 { font-size: 1.35rem; margin: 1.6rem 0 0.5rem; }
p { margin: 0 0 1rem; color: var(--ink); }
.lede { font-size: 1.2rem; color: var(--muted); }
label {
  display: block;
  font-family: "Segoe UI", Tahoma, sans-serif;
  font-size: 0.95rem;
  font-weight: 600;
  margin: 0.85rem 0 0.3rem;
}
input, textarea {
  width: 100%;
  font: inherit;
  color: var(--ink);
  background: #ffffff;
  border: 1px solid #8a8476;
  padding: 0.55rem 0.7rem;
}
input:focus, textarea:focus {
  outline: 3px solid #243447;
  outline-offset: 1px;
}
button {
  margin-top: 1rem;
  font-family: "Segoe UI", Tahoma, sans-serif;
  font-size: 1rem;
  font-weight: 600;
  color: #faf9f6;
  background: var(--nav);
  border: 0;
  padding: 0.65rem 1.15rem;
  cursor: pointer;
}
button:hover { background: var(--focus); }
.note { color: var(--muted); font-size: 1rem; }
footer {
  border-top: 1px solid var(--line);
  padding: 1.1rem 1.5rem 1.6rem;
  color: var(--muted);
  font-family: "Segoe UI", Tahoma, sans-serif;
  font-size: 0.95rem;
}
)CSS";

std::string html_page(const std::string& title,
                      const std::string& current,
                      const std::string& body) {
  auto nav_item = [&](const std::string& href, const std::string& label) {
    std::string extra = (current == href) ? " aria-current=\"page\"" : "";
    return "<a href=\"" + href + "\"" + extra + ">" + label + "</a>";
  };

  std::ostringstream out;
  out << "<!DOCTYPE html><html lang=\"en\"><head>"
      << "<meta charset=\"utf-8\">"
      << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
      << "<title>" << title << " · Quiet Pages</title>"
      << "<style>" << kCss << "</style>"
      << "</head><body>"
      << "<a class=\"skip\" href=\"#content\">Skip to content</a>"
      << "<header><div class=\"wrap\">"
      << "<p class=\"brand\">Quiet Pages</p>"
      << "<nav>"
      << nav_item("/", "Home")
      << nav_item("/about", "About")
      << nav_item("/contact", "Contact")
      << "</nav></div></header>"
      << "<main id=\"content\"><div class=\"wrap\"><div class=\"card\">"
      << body
      << "</div></div></main>"
      << "<footer><div class=\"wrap\">C++ site on port 8080 · muted colors, high-contrast text</div></footer>"
      << "</body></html>";
  return out.str();
}

std::string page_home() {
  return html_page("Home", "/",
    "<h1>A calm place to read</h1>"
    "<p class=\"lede\">This site uses a small C++ server, three pages, and a palette made for reading instead of decoration.</p>"
    "<p>The background is a warm gray. Body text is near-black. Links and buttons use one dark slate accent. Nothing flashes, and contrast stays high so sentences stay easy to follow.</p>"
    "<h2>What you can open</h2>"
    "<p><a href=\"/about\">About</a> explains the project. <a href=\"/contact\">Contact</a> has a short form.</p>");
}

std::string page_about() {
  return html_page("About", "/about",
    "<h1>About this project</h1>"
    "<p class=\"lede\">A Windows C++ program listens on a socket, reads a simple HTTP request, and returns HTML.</p>"
    "<p>Pages are built as strings in the server. The design is intentionally plain: serif type at 18px, 1.7 line height, and a narrow column so lines do not stretch across the whole screen.</p>"
    "<h2>Color choices</h2>"
    "<p>Warm paper, charcoal ink, one dark navigation bar. That keeps the page quiet while remaining easy to read in daylight.</p>");
}

std::string page_contact() {
  return html_page("Contact", "/contact",
    "<h1>Contact</h1>"
    "<p class=\"lede\">Send a note. This demo stores nothing; it only shows a thank-you page.</p>"
    "<form method=\"post\" action=\"/contact\">"
    "<label for=\"name\">Name</label>"
    "<input id=\"name\" name=\"name\" type=\"text\" required autocomplete=\"name\">"
    "<label for=\"email\">Email</label>"
    "<input id=\"email\" name=\"email\" type=\"email\" required autocomplete=\"email\">"
    "<label for=\"message\">Message</label>"
    "<textarea id=\"message\" name=\"message\" rows=\"6\" required></textarea>"
    "<button type=\"submit\">Send message</button>"
    "</form>");
}

std::string page_thanks() {
  return html_page("Message sent", "/contact",
    "<h1>Thank you</h1>"
    "<p>Your message was received by the demo server. Nothing was emailed or saved.</p>"
    "<p><a href=\"/\">Back to home</a></p>");
}

std::string page_not_found() {
  return html_page("Not found", "/",
    "<h1>Page not found</h1>"
    "<p>That URL is not one of Home, About, or Contact.</p>"
    "<p><a href=\"/\">Go home</a></p>");
}

std::string http_response(int status, const std::string& reason, const std::string& body) {
  std::ostringstream out;
  out << "HTTP/1.1 " << status << " " << reason << "\r\n"
      << "Content-Type: text/html; charset=utf-8\r\n"
      << "Content-Length: " << body.size() << "\r\n"
      << "Connection: close\r\n"
      << "\r\n"
      << body;
  return out.str();
}

std::string path_of(const std::string& request) {
  auto line_end = request.find("\r\n");
  std::string line = request.substr(0, line_end == std::string::npos ? request.size() : line_end);
  auto first = line.find(' ');
  auto second = line.find(' ', first == std::string::npos ? 0 : first + 1);
  if (first == std::string::npos || second == std::string::npos) {
    return "/";
  }
  std::string path = line.substr(first + 1, second - first - 1);
  auto q = path.find('?');
  if (q != std::string::npos) {
    path = path.substr(0, q);
  }
  return path;
}

std::string method_of(const std::string& request) {
  auto space = request.find(' ');
  if (space == std::string::npos) {
    return "GET";
  }
  return request.substr(0, space);
}

void send_all(SOCKET client, const std::string& data) {
  size_t sent_total = 0;
  while (sent_total < data.size()) {
    int sent = send(client, data.data() + sent_total,
                    static_cast<int>(data.size() - sent_total), 0);
    if (sent == SOCKET_ERROR) {
      return;
    }
    sent_total += static_cast<size_t>(sent);
  }
}

}  // namespace

int main() {
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    std::cerr << "WSAStartup failed\n";
    return 1;
  }

  SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server == INVALID_SOCKET) {
    std::cerr << "socket failed\n";
    WSACleanup();
    return 1;
  }

  BOOL reuse = TRUE;
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(static_cast<u_short>(kPort));
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if (bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
    std::cerr << "bind failed on port " << kPort << "\n";
    closesocket(server);
    WSACleanup();
    return 1;
  }

  if (listen(server, SOMAXCONN) == SOCKET_ERROR) {
    std::cerr << "listen failed\n";
    closesocket(server);
    WSACleanup();
    return 1;
  }

  std::cout << "Quiet Pages running at http://127.0.0.1:" << kPort << "\n"
            << "Routes: /  /about  /contact" << std::endl;

  for (;;) {
    SOCKET client = accept(server, nullptr, nullptr);
    if (client == INVALID_SOCKET) {
      continue;
    }

    char buffer[8192];
    int received = recv(client, buffer, sizeof(buffer) - 1, 0);
    if (received <= 0) {
      closesocket(client);
      continue;
    }
    buffer[received] = '\0';
    std::string request(buffer, received);

    const std::string method = method_of(request);
    const std::string path = path_of(request);

    std::string response;
    if (path == "/" || path == "/home") {
      response = http_response(200, "OK", page_home());
    } else if (path == "/about") {
      response = http_response(200, "OK", page_about());
    } else if (path == "/contact" && method == "POST") {
      response = http_response(200, "OK", page_thanks());
    } else if (path == "/contact") {
      response = http_response(200, "OK", page_contact());
    } else {
      response = http_response(404, "Not Found", page_not_found());
    }

    send_all(client, response);
    closesocket(client);
  }
}
