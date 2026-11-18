
#include <dpp/dpp.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <numeric>
#include <iostream>
#include <filesystem>
#include <cstdint>

using namespace std;

namespace fs = filesystem;


/*
    TODO
    ! recordar examenes y entregas
    ! coinflip ban o no ban (sobornable) y que envie solicitud
    *Cada x random tiempo te tire un sonido random en llamada
    /pedo --> pedo en llamada
    /pene --> imagenes random de penes ASCII
*/


#define DIRECTORIO_IMGS_RANDOM "C:/Users/orill/Desktop/Personal/Bot_DC/resources/random_img"
#define FICHERO_REFRANES "C:/Users/orill/Desktop/Personal/Bot_DC/resources/refranes.txt"

const dpp::snowflake GUILD_ID = 1570821952560566385;

//-----FUNCIONES AUXILIARES GENERALES-----//
vector<int> generarNumeros(int N, int max)
{
    if (N <= 0 || max <= 0)
        return {};

    vector<int> numeros(max);
    iota(numeros.begin(), numeros.end(), 0);

    static mt19937 gen(random_device{}());
    shuffle(numeros.begin(), numeros.end(), gen);

    numeros.resize(min(N, max));

    return numeros;
}
int contarElementosDirectorio(const string& s)
{
    int cantidad = std::distance(
        fs::directory_iterator(s),
        fs::directory_iterator{}
    );

    return cantidad;
}
void anadirComandos(dpp::cluster* bot)
{
    bot->on_ready([bot](const dpp::ready_t& event)
    {
        if (dpp::run_once<struct register_bot_commands>())
        {
            bot->global_bulk_command_delete();
            // PING
            bot->guild_command_create(
                dpp::slashcommand("ping", "Ping pong!", bot->me.id),
                GUILD_ID
            );

            // IMAGEN RANDOM
            dpp::slashcommand comando(
                "imagen_random",
                "Imagen random funny",
                bot->me.id
            );

            comando.add_option(
                dpp::command_option(
                    dpp::co_integer,
                    "numimgs",
                    "numero de imagenes a generar",
                    false
                )
            );

            bot->guild_command_create(comando, GUILD_ID);

            // SABIDURIA
            bot->guild_command_create(
                dpp::slashcommand(
                    "sabiduria",
                    "refran/proverbio",
                    bot->me.id
                ),
                GUILD_ID
            );

            std::cout << "Comandos de servidor registrados." << std::endl;
        }
    });
}


//-----FUNCIONES AUX COMANDOS-----//
// return name, path
vector<pair<string, string>> CargarFotos()
{
    vector<pair<string, string>> ImgName_path;

    for (const auto& archivo : fs::directory_iterator(DIRECTORIO_IMGS_RANDOM))
    {
        if (archivo.is_regular_file())
        {
            string extension = archivo.path().extension().string();

            if (extension == ".jpg" || extension == ".jpeg" ||
                extension == ".png" || extension == ".gif" ||
                extension == ".bmp" || extension == ".webp")
            {
                ImgName_path.push_back({
                    archivo.path().filename().string(),
                    archivo.path().string()
                });
            }
        }
    }

    return ImgName_path;
}

//-----IMG RANDOM-----//
dpp::message crearMensageImgsRandom(int nImagenes, vector<pair<string, string>> fotos)
{
    dpp::message mensaje;

    int maxFotos = static_cast<int>(fotos.size());

    vector<int> indicesFotos = generarNumeros(nImagenes, maxFotos);

    for (int indice : indicesFotos)
    {
        mensaje.add_file(
            fotos[indice].first,
            dpp::utility::read_file(fotos[indice].second)
        );
    }

    return mensaje;
}

//-----SABIDURIA-----//
vector<string> CargarRefranes(){

    vector<string> refranes;
    string refran;
    ifstream file(FICHERO_REFRANES);

    if (file.is_open()){
        while (getline(file,refran))
            refranes.push_back(refran);
    }
    return refranes;
}
string selecionarRefran(vector<string> refranes){
    int indice_refran = generarNumeros(1, refranes.size())[0];

    return refranes[indice_refran];
}

//-----SONIDOS RANDOM-----//



int main()
{
    const char* token = getenv("DISCORD_BOT_TOKEN");

    if (token == nullptr || token[0] == '\0')
    {
        cerr << "No se ha encontrado DISCORD_BOT_TOKEN\n";
        return 1;
    }

    //inicializar bot i elementos de memroia
    vector<pair<string, string>> fotos = CargarFotos();
    vector<string> refrenes = CargarRefranes();
    dpp::cluster bot(token);

    cout << "Fotos cargadas: " << fotos.size() << endl;
    cout << "Refranes cargados: " << refrenes.size() << endl;

    bot.on_log(dpp::utility::cout_logger());

    anadirComandos(&bot);

    bot.on_slashcommand([&fotos, &refrenes](const dpp::slashcommand_t& event)
    {
        string command = event.command.get_command_name();

        if (command == "ping")
        {
            event.reply("Pong!");
        }
        else if (command == "imagen_random")
        {
            int numImgs = 1;

            auto parametro = event.get_parameter("numimgs");

            if (holds_alternative<int64_t>(parametro))
                numImgs = static_cast<int>(get<int64_t>(parametro));

            if (numImgs <= 0)
            {
                event.reply("Introduce un numero mayor que 0.");
                return;
            }

            dpp::message msg = crearMensageImgsRandom(numImgs, fotos);

            event.reply(msg);
        }
        else if( command == "sabiduria"){
            string refran = "NULL";
            refran = selecionarRefran(refrenes);

            while (refran.empty())
                refran = selecionarRefran(refrenes);
            event.reply(refran);
        }


    });


    bot.start(dpp::st_wait);
}
