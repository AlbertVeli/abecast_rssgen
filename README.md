Custom tool to generate a static _feed.rss_ and _index.html_ for a
podcast. The feed and web page are created by scanning mp3- and
markdown-files in a target directory.


**REQUIREMENTS**

* **C-toolchain** (make, cc)
* **libid3tag** - http://www.underbit.com/products/mad/
* **dev-python/markdown** - https://pypi.python.org/pypi/Markdown
* **sed** and other standard tools available on Linux, OS X and other UNIX-like systems.


**INSTALLATION**

Fetch the files from this repository (using *git clone*) and copy
_make.inc.example_ to _make.inc_. Edit make.inc and change URL to
point to the file area of your webserver (if you have rsync access to
it).

Copy _metadata.h.example_ to _metadata.h_ and adjust the values in
metadata.h to describe your podcast.

Record at least one podcast episode and convert it to an mp3-file
(search to find recommended programs and parameters to encode
mp3-files for podcasting).

Create a directory named *target/*. All mp3-file(s) goes here.

Every mp3-file in the target directory must have a corresponding
markdown file. If the podcast is named *01-podcast.mp3*, then name the
markdown file *01-podcast.md*. You can use *podcast.md.example* (from
the base directory) as template.

Now edit the markdown file. It should begin with a comment:

    <--
      Title:    01 - First
      Subtitle: Adventures in Podland
      Author:   Podcastina
      Summary:  <![CDATA[Duke of Podcaster &amp; Fidel Podcastro discuss the rise and fall of homecasted pods.]]>
    -->

The values for **Title:**, **Subtitle:**, **Author:** and **Summary:**
are extracted from the comment to be inserted into the compiled
*feed.rss* file. Case matters.

There is not much error checking going on. All markdown files must
exist, have the required fields in the comment, the suffixes must be
.mp3 and .md respectively and the md-filename must match the name of
the mp3-file exactly, otherwise the script will fail in unexpected
ways.

Even now there might be problems because the RSS file doesn't handle
special characters very well. CDATA can be used to work around some of
these problems (as in the *Summary:* field in the example above).


**RUNNING**

Just type **make** (from a shell prompt).

This will build the *abecast_rssgen* tool and run the *generate.sh*
script. If all files in the target directory are in order the files
*feed.rss* and *index.html* file will be generated and placed in the
target directory. Edit generate.sh if you want to use another markdown
generator. The default one is markdown_py.

If you have rsync (or ssh) access to you webserver, you can enter
the address and path in make.inc and run **make sync** to upload the
whole target directory to the webserver.


**NOTES**

The target directory should also contain a .jpg or .png image (as
specified in metadata.h). iTunes wants a square image (1400x1400
according to the spec).

Comments are not supported (after all, the page is statically
generated). It is pretty easy though to add disqus comments by adding
a couple of lines to generate.sh (inside the for-loop, between the lines
*sed '0,/--&gt;/d' $i &gt;&gt; $HTML* and *echo "&lt;hr /&gt;" &gt;&gt; $HTML*).


**LICENSE**

    ~~=) All Rights Reversed - No Rights Reserved (=~~

Sweetmorn, the 49th day of The Aftermath in the YOLD 3179

Albert Veli
