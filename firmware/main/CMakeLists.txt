idf_component_register(
  SRCS
    "micro-dsp32.c"
    "wifi.c"
    "webserver.c"
    "config.c"

  EMBED_FILES
    "../../webapp/dist/gz/index.html.gz"
    "../../webapp/dist/gz/index.astro_astro_type_script_index_0_lang.js.gz"
    "../../webapp/dist/gz/index.astro_astro_type_script_index_0_lang2.js.gz"
    "../../webapp/dist/gz/global.js.gz"
  INCLUDE_DIRS "."
)
