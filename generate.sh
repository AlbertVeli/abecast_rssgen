#!/bin/sh

# python markdown
MARKDOWN="markdown_py -e 'utf-8'"
# Use this instead for John Gruber's original markdown
#MARKDOWN="markdown"

DIR=$1

if ! test -d "$DIR"; then
    echo "Usage: $0 <directory>"
    exit 1
fi

cd $DIR

get_header_tag()
{
    RE="^[[:space:]]*${1}[[:space:]]*"
    # Only first occurance
    sed "/${RE}/!d;q" $2 | sed "s/${RE}//"
}

get_metadata_tag()
{
    RE="^.*${1}"
    # path to metadata.h is currently hardcoded
    # sed lines at the end removes double quotes
    sed "/${RE}/!d;q" abecast_rssgen/metadata.h | \
	sed "s/${RE}//" | \
	sed 's/^[[:space:]]*"//' | \
	sed 's/"[[:space:]]*$//' | \
	sed 's/\//\\\//g'
}

for i in *.md; do
    html=`basename $i .md`.html
    $MARKDOWN $i > $html || exit 1
done

cd - > /dev/null


# Replace PODCAST_HOME/TITLE/SUBTITLE in template
HOME=`get_metadata_tag PODCAST_HOME`
TITLE=`get_metadata_tag PODCAST_TITLE `
SUBTITLE=`get_metadata_tag PODCAST_SUBTITLE`
LANGUAGE=`get_metadata_tag PODCAST_LANGUAGE`
SUMMARY=`get_metadata_tag PODCAST_SUMMARY`
KEYWORDS=`get_metadata_tag PODCAST_KEYWORDS`

FEED="${HOME}feed.rss"
sed "s/PODCAST_HOME/$HOME/g" header.html.template | \
    sed "s/PODCAST_FEED/$FEED/g" | \
    sed "s/PODCAST_SUMMARY/$SUMMARY/g" | \
    sed "s/PODCAST_KEYWORDS/$KEYWORDS/g" | \
    sed "s/PODCAST_LANGUAGE/$LANGUAGE/g" | \
    sed "s/PODCAST_TITLE/$TITLE/g" | \
    sed "s/PODCAST_SUBTITLE/$SUBTITLE/g" > header.html

# Replace SNOW_HTML line with snow.html block
sed '/SNOW_HTML/,$d' header.html > tmp.html
cat snow.html >> tmp.html
sed '/SNOW_HTML/,$!d' header.html | sed '/SNOW_HTML/d' >> tmp.html
mv tmp.html header.html

# Header
HTML="$DIR/index.html"
cp -f header.html $HTML

# Remove backslashs from $HOME
HOME=`echo "$HOME" | sed 's/\\\//g'`

# Loop through all episodes
for i in `find target -name '*.html' | sort -rn`; do
    if ! [ "$i" == "$HTML" ]; then
	echo $i;
	EP_TITLE=`get_header_tag "Title:" $i`
	EP_SUBTITLE=`get_header_tag "Subtitle:" $i`
	MP3=`basename $i .html`.mp3
	echo "<h2><a href=\"${HOME}${MP3}\">${EP_TITLE}</a></h2>" >> $HTML
	echo "<a href="https://itunes.apple.com/se/podcast/abecast/id777880403">iTunes</a> | <a href=\"${HOME}${MP3}\">mp3</a>" >> $HTML
	echo "<h3>${EP_SUBTITLE}</h3>" >> $HTML
	sed '/-->/,$!d' $i | sed '/-->/d' >> $HTML
	echo "<hr />" >> $HTML
	echo "" >> $HTML
    fi
done

# Insert disqus code (comment out these two lines to disable disqus)
SHORTNAME=`get_metadata_tag DISQUS_SHORTNAME`
sed "s/DISQUS_SHORTNAME/$SHORTNAME/g" disqus.template >> $HTML

# Footer
cat footer.html >> $HTML
