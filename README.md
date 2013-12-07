Custom tool to generate a static _feed.rss_ and _index.html_ for a
podcast. The feed and web page are created by scanning mp3- and
markdown-files in a target directory.


**REQUIREMENTS**

* **C-toolchain** (make, cc)
* **libid3tag** - http://www.underbit.com/products/mad/
* **dev-python/markdown** - https://pypi.python.org/pypi/Markdown
* **sed** and other standard tools available on Linux, OS X and other UNIX-like systems.


**INSTALLATION**

Get the files from this repository (using git) and copy
_make.inc.example_ to _make.inc_.  Change URL in make.inc to the file
area of your webserver.

Copy _metadata.h.example_ to _metadata.h_ and edit metadata.h to fit your podcast.

Record at least one podcast episode and convert it to an mp3-file.
Create a directory named _target/_ and put the mp3-file(s) there.

Create a corresponding markdown file for each mp3-file in the target
directory. If the podcast is named _01-podcast.mp3_ then name the
markdown file _01-podcast.md_. The names must match.

Edit the markdown file. It should begin with a comment formatted like this:

    <--
      Title:    01 - First
      Subtitle: Adventures in Podland
      Author:   Podcastina
      Summary:  <![CDATA[Duke of Podcaster and Fidel Podcastro discuss the rise and fall of homecasted pods.]]>
    -->

The values for Title:, Subtitle: Author: and Summary: (case matters)
are extracted from the comment to be inserted into the compiled
_feed.rss_ file.

There is not much error checking going on. All markdown files must
exist, have the required fields and the filename must match the name
of the mp3-file exactly, otherwise the script will fail in unexpected
ways.


**RUNNING**

Just type _make_ (from a shell prompt).

This will build _abecast_rssgen_ and run the _generate.sh_ script. If
all files in the target directory are in order a resulting feed.rss
file will be built and an _index.html_ file will be generated (by
markdown_py).

If you have rsync (or ssh) access to you webserver, you can enter
the address and path in make.inc and run _make sync_ to upload the
whole target directory to the webserver.

Note. The target directory should also contain a .jpg or .png image
(as specified in metadata.h). iTunes wants a square image (1400x1400
according to the spec).


**LICENSE**

    ~~=) All Rights Reversed - No Rights Reserved (=~~

Sweetmorn, the 49th day of The Aftermath in the YOLD 3179

Albert Veli
