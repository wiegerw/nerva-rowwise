OUTPUT_DIRECTORY=../output_directory
mkdir -p $OUTPUT_DIRECTORY
asciidoctor -r asciidoctor-bibtex -a source-highlighter=rouge -D $OUTPUT_DIRECTORY ../doc/*.adoc
