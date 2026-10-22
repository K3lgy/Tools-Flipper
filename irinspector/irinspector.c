/*
 * IR Inspector - Audit infrarouge de telecommandes de laboratoire
 * Outil de laboratoire pour l'inventaire de telecommandes IR en environnement de test. Usage reserve a du materiel /
 * environnement de test AUTORISE.
 *
 * Carnet embarque : identifiant de session, cible/perimetre autorise,
 * journal d'observations horodatees, checklist, export via ordinateur
 * (tools/export_report.py).
 *
 * NB : ce squelette fournit la structure d'application (.fap) et la
 * logique de journalisation/export. Les appels bas niveau specifiques
 * au sous-systeme Flipper (infrarouge (recepteur/emetteur IR)) sont a brancher dans
 * capture_signal() selon l'API exacte de ta version de firmware/SDK.
 */
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/text_box.h>
#include <gui/modules/variable_item_list.h>
#include <storage/storage.h>
#include <stdlib.h>
#include <string.h>

#define DATA_DIR  "/ext/apps_data/irinspector"
#define DATA_FILE DATA_DIR "/session.txt"
#define TEXT_LEN  64
#define INPUT_LEN 96
#define MAX_FILE  8192

typedef enum { ViewMenu, ViewInput, ViewChecklist, ViewBox } ViewId;
typedef enum { MenuSession, MenuTarget, MenuCapture, MenuNote, MenuChecklist, MenuLog, MenuSave } MenuIndex;
typedef enum { InputSession, InputTarget, InputNote } InputMode;

static const char* const CHECKS[] = {
    "Reconnaissance du protocole IR",
    "Catalogage de l'appareil (marque/modele)",
    "Verification de l'autorisation d'emission",
    "Test d'emission sur equipement autorise uniquement",
    "Journal du catalogue local",
    "Export du catalogue",
};
#define CHECK_COUNT (sizeof(CHECKS) / sizeof(CHECKS[0]))

typedef struct {
    Gui* gui;
    Storage* storage;
    ViewDispatcher* vd;
    Submenu* menu;
    TextInput* input;
    TextBox* box;
    VariableItemList* list;

    char session[TEXT_LEN];
    char target[TEXT_LEN];
    char input_buf[INPUT_LEN];
    bool checks[CHECK_COUNT];
    VariableItem* items[CHECK_COUNT];
    InputMode mode;

    FuriString* notes;
    FuriString* box_text;
} App;

/* ---------- Capture (a completer selon l'API materielle) ---------- */

static void capture_signal(App* app) {
    DateTime dt;
    furi_hal_rtc_get_datetime(&dt);
    /* TODO : brancher ici l'acquisition infrarouge (recepteur/emetteur IR) reelle et en tirer
       une ligne de synthese (type, protocole, frequence/UID/etc. selon
       le sous-systeme) a la place du texte generique ci-dessous. */
    furi_string_cat_printf(
        app->notes,
        "NOTE=[%04d-%02d-%02d %02d:%02d] Capture manuelle - a completer (voir capture_signal)\n",
        dt.year, dt.month, dt.day, dt.hour, dt.minute);
}

/* ---------- Persistance ---------- */

static bool save_session(App* app) {
    storage_simply_mkdir(app->storage, "/ext/apps_data");
    storage_simply_mkdir(app->storage, DATA_DIR);
    File* f = storage_file_alloc(app->storage);
    bool ok = storage_file_open(f, DATA_FILE, FSAM_WRITE, FSOM_CREATE_ALWAYS);
    if(ok) {
        FuriString* s = furi_string_alloc();
        furi_string_printf(s, "SESSION=%s\nTARGET=%s\nCHECK=", app->session, app->target);
        for(size_t i = 0; i < CHECK_COUNT; i++)
            furi_string_push_back(s, app->checks[i] ? '1' : '0');
        furi_string_cat_printf(s, "\n%s", furi_string_get_cstr(app->notes));
        ok = storage_file_write(f, furi_string_get_cstr(s), furi_string_size(s)) == furi_string_size(s);
        furi_string_free(s);
    }
    storage_file_close(f);
    storage_file_free(f);
    return ok;
}

static void load_session(App* app) {
    File* f = storage_file_alloc(app->storage);
    if(storage_file_open(f, DATA_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint64_t size = storage_file_size(f);
        if(size > MAX_FILE) size = MAX_FILE;
        char* buf = malloc((size_t)size + 1);
        size_t n = storage_file_read(f, buf, (size_t)size);
        buf[n] = '\0';
        char* line = buf;
        while(line && *line) {
            char* nl = strchr(line, '\n');
            if(nl) *nl = '\0';
            if(strncmp(line, "SESSION=", 8) == 0) strlcpy(app->session, line + 8, TEXT_LEN);
            else if(strncmp(line, "TARGET=", 7) == 0) strlcpy(app->target, line + 7, TEXT_LEN);
            else if(strncmp(line, "CHECK=", 6) == 0) {
                for(size_t i = 0; i < CHECK_COUNT && line[6 + i]; i++)
                    app->checks[i] = (line[6 + i] == '1');
            } else if(strncmp(line, "NOTE=", 5) == 0) {
                furi_string_cat_printf(app->notes, "%s\n", line);
            }
            line = nl ? nl + 1 : NULL;
        }
        free(buf);
    }
    storage_file_close(f);
    storage_file_free(f);
}

/* ---------- Saisie ---------- */

static void input_done(void* ctx) {
    App* app = ctx;
    switch(app->mode) {
    case InputSession: strlcpy(app->session, app->input_buf, TEXT_LEN); break;
    case InputTarget: strlcpy(app->target, app->input_buf, TEXT_LEN); break;
    case InputNote:
        if(app->input_buf[0]) {
            DateTime dt;
            furi_hal_rtc_get_datetime(&dt);
            furi_string_cat_printf(
                app->notes, "NOTE=[%04d-%02d-%02d %02d:%02d] %s\n",
                dt.year, dt.month, dt.day, dt.hour, dt.minute, app->input_buf);
        }
        break;
    }
    view_dispatcher_switch_to_view(app->vd, ViewMenu);
}

static void start_input(App* app, InputMode mode) {
    app->mode = mode;
    const char* header = "";
    app->input_buf[0] = '\0';
    bool clear_default = true;
    if(mode == InputSession) {
        header = "ID de session";
        strlcpy(app->input_buf, app->session, TEXT_LEN);
        clear_default = false;
    } else if(mode == InputTarget) {
        header = "Environnement de test autorise";
        strlcpy(app->input_buf, app->target, TEXT_LEN);
        clear_default = false;
    } else {
        header = "Observation";
    }
    text_input_reset(app->input);
    text_input_set_header_text(app->input, header);
    text_input_set_result_callback(app->input, input_done, app, app->input_buf, INPUT_LEN, clear_default);
    view_dispatcher_switch_to_view(app->vd, ViewInput);
}

/* ---------- Affichage ---------- */

static void show_text(App* app, const char* text) {
    furi_string_set(app->box_text, text);
    text_box_reset(app->box);
    text_box_set_font(app->box, TextBoxFontText);
    text_box_set_text(app->box, furi_string_get_cstr(app->box_text));
    view_dispatcher_switch_to_view(app->vd, ViewBox);
}

static void show_log(App* app) {
    FuriString* tmp = furi_string_alloc();
    furi_string_printf(
        tmp, "ID de session: %s\nEnvironnement de test autorise: %s\n--------\n",
        app->session[0] ? app->session : "(non defini)",
        app->target[0] ? app->target : "(non defini)");
    if(furi_string_empty(app->notes)) {
        furi_string_cat_str(tmp, "(aucune observation)");
    } else {
        FuriString* n = furi_string_alloc_set(app->notes);
        furi_string_replace_all(n, "NOTE=", "");
        furi_string_cat(tmp, n);
        furi_string_free(n);
    }
    show_text(app, furi_string_get_cstr(tmp));
    furi_string_free(tmp);
}

static void check_changed(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t v = variable_item_get_current_value_index(item);
    for(size_t i = 0; i < CHECK_COUNT; i++) {
        if(app->items[i] == item) { app->checks[i] = (v == 1); break; }
    }
    variable_item_set_current_value_text(item, v ? "[X]" : "[ ]");
}

static void menu_callback(void* ctx, uint32_t index) {
    App* app = ctx;
    switch(index) {
    case MenuSession: start_input(app, InputSession); break;
    case MenuTarget: start_input(app, InputTarget); break;
    case MenuCapture: capture_signal(app); show_text(app, "Capture enregistree dans le journal.\n(A completer : voir capture_signal() dans le code)"); break;
    case MenuNote: start_input(app, InputNote); break;
    case MenuChecklist: view_dispatcher_switch_to_view(app->vd, ViewChecklist); break;
    case MenuLog: show_log(app); break;
    case MenuSave: {
        bool ok = save_session(app);
        show_text(app, ok ? "Sauvegarde OK\n\n" DATA_FILE
                             "\n\nRecupere le fichier puis lance tools/export_report.py."
                          : "Echec de la sauvegarde.\nCarte SD presente ?");
        break;
    }
    }
}

static uint32_t back_to_menu(void* ctx) { UNUSED(ctx); return ViewMenu; }
static uint32_t exit_app(void* ctx) { UNUSED(ctx); return VIEW_NONE; }

static App* app_alloc(void) {
    App* app = malloc(sizeof(App));
    memset(app, 0, sizeof(App));
    app->notes = furi_string_alloc();
    app->box_text = furi_string_alloc();
    app->gui = furi_record_open(RECORD_GUI);
    app->storage = furi_record_open(RECORD_STORAGE);
    load_session(app);

    app->vd = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(app->vd, app->gui, ViewDispatcherTypeFullscreen);

    app->menu = submenu_alloc();
    submenu_set_header(app->menu, "IR Inspector");
    submenu_add_item(app->menu, "ID de session", MenuSession, menu_callback, app);
    submenu_add_item(app->menu, "Environnement de test autorise", MenuTarget, menu_callback, app);
    submenu_add_item(app->menu, "Noter protocole detecte (manuel)", MenuCapture, menu_callback, app);
    submenu_add_item(app->menu, "Ajouter une observation", MenuNote, menu_callback, app);
    submenu_add_item(app->menu, "Checklist", MenuChecklist, menu_callback, app);
    submenu_add_item(app->menu, "Voir journal", MenuLog, menu_callback, app);
    submenu_add_item(app->menu, "Sauvegarder (export)", MenuSave, menu_callback, app);
    view_set_previous_callback(submenu_get_view(app->menu), exit_app);
    view_dispatcher_add_view(app->vd, ViewMenu, submenu_get_view(app->menu));

    app->input = text_input_alloc();
    view_set_previous_callback(text_input_get_view(app->input), back_to_menu);
    view_dispatcher_add_view(app->vd, ViewInput, text_input_get_view(app->input));

    app->list = variable_item_list_alloc();
    for(size_t i = 0; i < CHECK_COUNT; i++) {
        VariableItem* it = variable_item_list_add(app->list, CHECKS[i], 2, check_changed, app);
        variable_item_set_current_value_index(it, app->checks[i] ? 1 : 0);
        variable_item_set_current_value_text(it, app->checks[i] ? "[X]" : "[ ]");
        app->items[i] = it;
    }
    view_set_previous_callback(variable_item_list_get_view(app->list), back_to_menu);
    view_dispatcher_add_view(app->vd, ViewChecklist, variable_item_list_get_view(app->list));

    app->box = text_box_alloc();
    view_set_previous_callback(text_box_get_view(app->box), back_to_menu);
    view_dispatcher_add_view(app->vd, ViewBox, text_box_get_view(app->box));

    view_dispatcher_switch_to_view(app->vd, ViewMenu);
    return app;
}

static void app_free(App* app) {
    view_dispatcher_remove_view(app->vd, ViewMenu);
    view_dispatcher_remove_view(app->vd, ViewInput);
    view_dispatcher_remove_view(app->vd, ViewChecklist);
    view_dispatcher_remove_view(app->vd, ViewBox);
    submenu_free(app->menu);
    text_input_free(app->input);
    variable_item_list_free(app->list);
    text_box_free(app->box);
    view_dispatcher_free(app->vd);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_GUI);
    furi_string_free(app->notes);
    furi_string_free(app->box_text);
    free(app);
}

int32_t irinspector_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();
    view_dispatcher_run(app->vd);
    app_free(app);
    return 0;
}
