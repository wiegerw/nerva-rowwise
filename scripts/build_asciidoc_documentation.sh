OUTPUT_DIRECTORY=../output_directory
mkdir -p $OUTPUT_DIRECTORY
asciidoctor -r asciidoctor-bibtex -D $OUTPUT_DIRECTORY ../doc/*.adoc
