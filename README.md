# cmdimgtool
Command line tool for common image operations, such as resize, extract and overlay. The goal is to enable batch editing of several image files. The tool itself only performs singular operations, but it's easy to chain or list them in batch files or with a for command.

This program uses the webp library created and shared by Google and used according to terms described in the Creative Commons 4.0 Attribution License. See https://developers.google.com/speed/webp.

Uses:

cmdimgtool extract src-file dst-file x y dxSrc dySrc [dxDst] [dyDst]

 - Extract a part of the image in src-file. The extracted image
   will be a [dx:dy] sized chunk starting at [x:y].
   The extracted image will be saved to dstfile. The dimensions of
   the destination image will be [dxDst:dyDst], defaulting to [dx:dy].

cmdimgtool resize src-file dst-file dxDst dyDst

 - Resize src-file to dst-file with the new dimections dxDst:dyDst.
 
