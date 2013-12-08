/* Copyleft - All Rights Reversed */

/* Simple program to generate an itunes-compatible
 * rss-feed for podcasts. Give a directory name as input.
 * The program will scan the directory (no subdirs)
 * for .mp3 and .pdf files and print the rss feed to
 * stdout.
 *
 * Length is read from the id3-tag of the mp3 file. Date
 * is taken from the last modified date for each file.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include <id3tag.h> /* libid3tag used to read length of mp3 */
#include <unistd.h>

/* TODO: Read metadata dynamically from file instead */
#include "metadata.h"

#define GENERATOR "https://github.com/AlbertVeli/abecast_rssgen"

/* Assume all bufs can hold MAXBUF bytes */
#define MAXBUF 1024

/* Globals */
static char g_title[MAXBUF];
static char g_subtitle[MAXBUF];
static char g_author[MAXBUF];
static char g_summary[MAXBUF];

/* Memory mapping */
static char *map = NULL;
static struct stat mapstat;


static void free_map(void)
{
   if (map) {
      munmap(map, mapstat.st_size);
      map = NULL;
   }
}

static int init_map(const char *filename)
{
   int ret = 0;
   int fd = open(filename, O_RDONLY);

   if (fd < 0) {
      perror(filename);
      goto out;
   }

   if (fstat(fd, &mapstat) < 0) {
      perror(filename);
      goto out2;
   }

   map = mmap(NULL, mapstat.st_size, PROT_READ, MAP_SHARED, fd, 0);
   if (map == MAP_FAILED) {
      perror(filename);
      goto out2;
   }

   ret = 1;

 out2:

   close(fd);

 out:

   return ret;
}

/* Probably no need to change anything below this line */

static void basic_tag(int tabs, const char *tag, const char *content)
{
   int i;

   for (i = 0; i < tabs; i++) {
      printf("\t");
   }

   printf("<%s>%s</%s>\n", tag, content, tag);
}

static void one_nocontent_tag(int tabs, const char *tag, const char *key, const char *val)
{
   int i;

   for (i = 0; i < tabs; i++) {
      printf("\t");
   }

   printf("<%s %s=\"%s\" />\n", tag, key, val);
}

static void l1_category(int tabs, const char *category)
{
   int i;

   for (i = 0; i < tabs; i++) {
      printf("\t");
   }
   printf("<itunes:category text=\"%s\" />\n", category);
}

static void l2_category(int tabs, const char *c1, const char *c2)
{
   int i;

   for (i = 0; i < tabs; i++) {
      printf("\t");
   }
   printf("<itunes:category text=\"%s\">\n", c1);

   l1_category(tabs + 1, c2);

   for (i = 0; i < tabs; i++) {
      printf("\t");
   }
   printf("</itunes:category>\n");
}


static void date_tag(int tabs, const char *tag, time_t t)
{
   struct tm tm = *gmtime(&t);
   char datestr[256];

   strftime(datestr, 255, "%a, %d %b %Y %T +0000", &tm);
   basic_tag(tabs, tag, datestr);
}

static void rss_header(void)
{
   printf(
          "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
          "<rss version=\"2.0\"\n"
          "\txmlns:content=\"http://purl.org/rss/1.0/modules/content/\"\n"
          "\txmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
          "\txmlns:atom=\"http://www.w3.org/2005/Atom\"\n"
          "\txmlns:sy=\"http://purl.org/rss/1.0/modules/syndication/\"\n"
          "\txmlns:itunes=\"http://www.itunes.com/dtds/podcast-1.0.dtd\"\n"
          ">\n");
   printf("<channel>\n");
   basic_tag(1, "title", PODCAST_TITLE);
   printf("\t<atom:link href=\"%s\" rel=\"self\" type=\"application/rss+xml\" />\n", PODCAST_FEED);
   basic_tag(1, "link", PODCAST_HOME);
   /* lastBuildDate */
   date_tag(1, "lastBuildDate", time(NULL));
   basic_tag(1, "language", PODCAST_LANGUAGE);
   basic_tag(1, "sy:updatePeriod", "hourly");
   basic_tag(1, "sy:updateFrequency", "1");
   basic_tag(1, "generator", GENERATOR);
   basic_tag(1, "copyright", PODCAST_COPYRIGHT);
   printf("\t<image>\n");
   basic_tag(2, "url", PODCAST_LOGO);
   basic_tag(2, "title", PODCAST_TITLE);
   basic_tag(2, "link", PODCAST_HOME);
   basic_tag(2, "width", "144");
   basic_tag(2, "height", "144");
   printf("\t</image>\n");
   basic_tag(1, "itunes:subtitle", PODCAST_SUBTITLE);
   basic_tag(1, "itunes:author", PODCAST_AUTHOR);
   basic_tag(1, "itunes:summary", PODCAST_SUMMARY);
   basic_tag(1, "description", PODCAST_SUMMARY);
   printf("\t<itunes:owner>\n");
   basic_tag(2, "itunes:name", PODCAST_AUTHOR);
   basic_tag(2, "itunes:email", PODCAST_EMAIL);
   printf("\t</itunes:owner>\n");
   one_nocontent_tag(1, "itunes:image", "href", PODCAST_LOGO);

   /* Meta categories (should not be hard coded) */
   basic_tag(1, "itunes:keywords", PODCAST_KEYWORDS);
   l2_category(1, "Games &amp; Hobbies", "Video Games");
   l2_category(1, "Technology", "Tech News");
   l1_category(1, "TV &amp; Film");
   basic_tag(1, "itunes:explicit", "clean");
}


static void rss_footer(void)
{
   printf("</channel>\n</rss>\n");
}

static void rss_item(const char *title, const char *subtitle, const char *author, const char *summary, const char *encoded_description, const char *url, unsigned int seconds, unsigned long length, time_t pubdate, const char *mimetype)
{
   char duration[64];

   printf("\t<item>\n");
   basic_tag(2, "title", title);
   basic_tag(2, "itunes:author", author);
   basic_tag(2, "itunes:subtitle", subtitle);
   basic_tag(2, "itunes:summary", summary);
   basic_tag(2, "description", summary);
   printf("\t\t\t<content:encoded><![CDATA[%s]]></content:encoded>\n", encoded_description);
   printf("\t\t<enclosure url=\"%s\" length=\"%ld\" type=\"%s\" />\n", url, length, mimetype);
   basic_tag(2, "guid", url); /* unique identifier */
   date_tag(2, "pubDate", pubdate);
   snprintf(duration, 63, "%d", seconds);
   basic_tag(2, "itunes:duration", duration); /* unique identifier */
   printf("\t</item>\n");
}

static int mp3_len(const char *filename)
{
   struct id3_tag *tag;
   struct id3_file *id3 = id3_file_open(filename, ID3_FILE_MODE_READONLY);
   int len = 0;

   if (!id3) {
      return 0;
   }

   tag = id3_file_tag(id3);
   if (tag) {
      struct id3_frame *frame;
      frame = id3_tag_findframe(tag, "TLEN", 0);
      if (frame) {
         int i;
         int sum = 0;
         union id3_field *field = id3_frame_field(frame, 1);
         id3_ucs4_t *s = (id3_ucs4_t *)id3_field_getstrings(field, 0);
         i = 0;
         /* s = milliseconds in strange "uint32_t *" string */
         while (s[i] != 0) {
            sum *= 10;
            sum += s[i] - 0x30;
            i++;
         }
         /* len = seconds */
         len = sum / 1000;
      }
   }
   id3_file_close(id3);

   return len;
}

static int get_meta(const char *filename, const char *key, char *value)
{
   char line[MAXBUF];
   FILE *fp = fopen(filename, "r");

   if (!fp) {
      perror(filename);
      return 0;
   }

   while(fgets(line, MAXBUF - 1, fp)) {
      char *p = strstr(line, key);
      if (p) {
         /* Found it */
         p += strlen(key);
         /* Skip whitespace */
         while (*p == ' ' || *p == '\t') {
            p++;
         }
         strncpy(value, p, MAXBUF - 1);
         /* Remove trailing newline/whitespace */
         p = value + strlen(value) - 1;
         if (*p == '\n') {
            *p-- = 0;
         }
         while (*p == ' ' || *p == '\t') {
            *p-- = 0;
         }
         fclose(fp);
         return 1;
      }
   }
   fclose(fp);
   return 0;
}

static int is_dir(const char *path)
{
   struct stat st;
   if (stat(path, &st) != 0) {
      return 0;
   }
   if (S_ISDIR(st.st_mode)) {
      return 1;
   }

   return 0;
}

static void add_items(const char *dir)
{
   DIR *dp;
   struct dirent *ep;
   struct stat st;
   int seconds;
   char *p_description = NULL;
   char path[MAXBUF];

   dp = opendir (dir);
   if (!dp) {
      perror(dir);
      return;
   }

   while ((ep = readdir(dp))) {
      snprintf(path, MAXBUF, "%s/%s", dir, ep->d_name);
      if (!stat(path, &st)) {
         char *suffix;
         suffix = strrchr(ep->d_name, '.');
         if (suffix && (!strcmp(suffix, ".mp3"))) {
            seconds = mp3_len(path);
            /* Set path to d_name - suffix .html */
            snprintf(path, MAXBUF, "%s/%s", dir, ep->d_name);
            suffix = strrchr(path, '.');
            *suffix = 0;
            strncat(path, ".html", MAXBUF - 1);

            /* Read title, subtitle, author and description from path here */
            if (!get_meta(path, "Title:", g_title)) {
               strcpy(g_title, "Default Title");
            }
            if (!get_meta(path, "Subtitle:", g_subtitle)) {
               strcpy(g_subtitle, "Default Subtitle");
            }
            if (!get_meta(path, "Author:", g_author)) {
               strcpy(g_author, "Default Author");
            }
            if (!get_meta(path, "Summary:", g_summary)) {
               strcpy(g_summary, "<![CDATA[Default Summary]]>");
            }

            /* mmap html-file */
            if (!init_map(path)) {
               p_description = "Default description";
            } else {
               /* forward p_description past initial comment + whitespace */
               p_description = strstr(map, "-->");
            }
            if (p_description) {
               p_description += 3;
               while (*p_description == ' ' || *p_description == '\t' || *p_description == '\n') {
                  p_description++;
               }
            } else {
               p_description = "Default description";
            }
            /* Set path to full mp3 url */
            snprintf(path, MAXBUF, "%s%s", PODCAST_HOME, ep->d_name);
            rss_item(g_title, g_subtitle, g_author, g_summary, p_description,
                     path, seconds, (unsigned long)st.st_size, st.st_mtime,
                     "audio/mpeg" /* .mp3 = audio/mpeg, .pdf = application/pdf */
                     );
            free_map();
         }
      }
   }

   closedir(dp);
}

int main(int argc, char *argv[])
{
   if (argc != 2) {
      fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
      return 1;
   }

   rss_header();

   add_items("target");

   rss_footer();

   return 0;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
