from random import randrange
import sys


def main():
    print(len(pre))
    return

    b = bytes([randrange(256) for i in range(128)])
    if len(sys.argv) > 1:
        title = sys.argv[1]
    else:
        title = 'random-bg.html'

    if not title.lower().endswith('.html'):
        title += '.html'

    with open(title, 'bw+') as f:
        f.write(pre)
        f.write(b)
        f.write(post)


pre = b'''<!DOCTYPE html>
<html lang="en">

<head>
  <!--padding-->
  <meta charset="UTF-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>MD5 Attack Sample</title>
</head>

<body>
  <main>
    <h1>MD5 Collision Attack</h1>
    <p>This file has a unique background color.</p>
    <pre id="seed" hidden>'''
post = b'''
</pre>
  </main>
  <style>
    * {
      box-sizing: border-box;
    }

    main {
      padding: 20px;
      margin: auto;
      background-color: white;
      width: 60%;
      height: 400px;
    }

    body {
      margin: auto;
      background-color: blue;
      height: 100vh;
    }
  </style>
  <script>
    function toHex(d) {
      return ("0" + (Number(d).toString(16))).slice(-2).toUpperCase()
    }

    let seedPre = document.getElementById("seed");
    let s = seedPre.innerHTML;
    let r = s.charCodeAt(0);
    let g = s.charCodeAt(1);
    let b = s.charCodeAt(2);

    let rgb = `#${toHex(r)}${toHex(g)}${toHex(b)}`;

    console.log('bg rgb', rgb);
    document.body.style.backgroundColor = rgb;
  </script>
</body>

</html>
'''

if __name__ == '__main__':
    main()
