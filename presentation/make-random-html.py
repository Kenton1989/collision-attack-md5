from random import randrange
import sys


def main():
    print(len(pre))

    b0 = bytes.fromhex('0d60d29d26bcec8b323ffb359b76746c21e83c7ab01f16d169a5b57c329a2202c12c3406fcfd53035bbb6c82d5d6d1d70ca7c9ecf58b79dc1d321b856533f58df6d4ef9e14255ff32f8def42d6f56fbc8497c8a473ff67ec0c2d6f95529efdb0e537bbb1b97ebe4a8ed097a88f74ad7ebdb46f346cb72b81d6ffc1fe5dcde769')
    title0 = 'm0.html'
    with open(title0, 'bw+') as f:
        f.write(pre)
        f.write(b0)
        f.write(post)

    b1 = bytes.fromhex('0d60d29d26bcec8b323ffb359b76746c21e83cfab01f16d169a5b57c329a2202c12c3406fcfd53035bbb6c82d556d2d70ca7c9ecf58b79dc1d321b056533f58df6d4ef9e14255ff32f8def42d6f56fbc8497c82473ff67ec0c2d6f95529efdb0e537bbb1b97ebe4a8ed097a88ff4ac7ebdb46f346cb72b81d6ffc17e5dcde769')
    title1 = 'm1.html'
    with open(title1, 'bw+') as f:
        f.write(pre)
        f.write(b1)
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
    function sfc32(a, b, c, d) {
      return function() {
        a >>>= 0; b >>>= 0; c >>>= 0; d >>>= 0; 
        var t = (a + b) | 0;
        a = b ^ b >>> 9;
        b = c + (c << 3) | 0;
        c = (c << 21 | c >>> 11);
        d = d + 1 | 0;
        t = t + d | 0;
        c = c + t | 0;
        return (t >>> 0) / 4294967296;
      }
    }

    let seedPre = document.getElementById("seed");
    let s = seedPre.innerHTML;
    let c20 = s.charCodeAt(20);
    let c24 = s.charCodeAt(24);
    let rand = sfc32(c20, c24, c20, c24);
    let r = rand()%256;
    let g = rand()%256;
    let b = rand()%256;

    let rgb = `#${toHex(r)}${toHex(g)}${toHex(b)}`;

    console.log('bg rgb', rgb);
    document.body.style.backgroundColor = rgb;
  </script>
</body>

</html>
'''

if __name__ == '__main__':
    main()
