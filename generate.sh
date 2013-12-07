#!/bin/sh

DIR=$1

if ! test -d "$DIR"; then
    echo "Usage: $0 <directory>"
    exit 1
fi

cd $DIR

get_header_tag()
{
    RE="^[ \t]*${1}[ \t]*"
    # Only first occurance
    sed -n "0,/${RE}/s///p" $2
}

get_metadata_tag()
{
    RE="^.*${1}[ \t]*"
    # path to metadata.h is currently hardcoded
    # sed lines at the end removes double quotes
    sed -n "0,/${RE}/s///p" abecast_rssgen/metadata.h | \
	sed 's/^[ \t]*"//' | \
	sed 's/"[ \t]*$//' | \
	sed 's/\//\\\//g'
}

for i in *.md; do
    html=`basename $i .md`.html
    markdown_py -e 'utf-8' $i > $html
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

# Replace SNOW_HTML line with snowout.html block
sed '0,/SNOW_HTML/!d' header.html | sed '/SNOW_HTML/d' > tmp.html
cat snow.html >> tmp.html
sed '0,/SNOW_HTML/d' header.html >> tmp.html
mv tmp.html header.html

# Header
HTML="$DIR/index.html"
cp -f header.html $HTML

# Remove backslashs from $HOME
HOME=`echo "$HOME" | sed 's/\\\//g'`

# Loop through all episodes
for i in `ls -r $DIR/*.html`; do
    if ! [ "$i" == "$HTML" ]; then
	echo $i;
	EP_TITLE=`get_header_tag "Title:" $i`
	EP_SUBTITLE=`get_header_tag "Subtitle:" $i`
	MP3=`basename $i .html`.mp3
	echo "<h2><a href=\"${HOME}${MP3}\">${EP_TITLE}</a></h2>" >> $HTML
	echo "iTunes | <a href=\"${HOME}${MP3}\">mp3</a>" >> $HTML
	echo "<h3>${EP_SUBTITLE}</h3>" >> $HTML
	sed '0,/-->/d' $i >> $HTML
	echo "<hr />" >> $HTML
	echo "" >> $HTML
    fi
done

# Insert disqus code (comment out these two lines to disable disqus)
SHORTNAME=`get_metadata_tag DISQUS_SHORTNAME`
sed "s/DISQUS_SHORTNAME/$SHORTNAME/g" disqus.template >> $HTML

# Footer
cat footer.html >> $HTML
