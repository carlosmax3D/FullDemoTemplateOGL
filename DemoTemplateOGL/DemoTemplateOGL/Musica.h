#pragma once
#ifdef _WIN32 
#include <windows.h>
#include <io.h>
#else
#include <dirent.h>   // opendir, readdir, closedir
#include <sys/types.h>
#include <cstring>    // strcmp
#endif
#include "bass.h"
#include <vector>
#include <string>
#include <random>
#include <algorithm>

class Musica {
private:
    HSTREAM musicaStream = 0;
    std::vector<HSAMPLE> efectosSamples;
    std::vector<HCHANNEL> efectosChannels;
    std::vector<std::string> listaMusica;
    std::vector<int> playlistAleatoria;
    int indicePlaylist = -1;
    bool reproduciendo = false;
    bool primeraReproduccion = true;
    float volumenMusica = 0.50f; // Volumen inicial al 5%
    float volumenEfectos = 0.50f; // Volumen de efectos

    std::string obtenerNombreArchivo(const std::string& rutaCompleta) const {
        size_t ultimaBarra = rutaCompleta.find_last_of("\\/");
        if (ultimaBarra != std::string::npos) {
            return rutaCompleta.substr(ultimaBarra + 1);
        }
        return rutaCompleta;
    }

    std::string obtenerExtension(const std::string& archivo) const {
        size_t punto = archivo.find_last_of(".");
        if (punto != std::string::npos) {
            std::string ext = archivo.substr(punto);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            return ext;
        }
        return "";
    }

    void generarPlaylistAleatoria() {
        playlistAleatoria.clear();
        for (int i = 0; i < listaMusica.size(); i++) {
            playlistAleatoria.push_back(i);
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(playlistAleatoria.begin(), playlistAleatoria.end(), g);

        indicePlaylist = -1;
    }

    // Aplicar volumen actual al stream de m�sica
    void aplicarVolumenMusica() {
        if (musicaStream) {
            BASS_ChannelSetAttribute(musicaStream, BASS_ATTRIB_VOL, volumenMusica);
        }
    }

public:
    bool iniciar() {
        return BASS_Init(-1, 44100, 0, 0, NULL);
    }

    void cerrar() {
        if (musicaStream) {
            BASS_ChannelStop(musicaStream);
            BASS_StreamFree(musicaStream);
            musicaStream = 0;
        }

        for (HCHANNEL channel : efectosChannels) {
            BASS_ChannelStop(channel);
        }
        for (HSAMPLE sample : efectosSamples) {
            BASS_SampleFree(sample);
        }

        BASS_Free();
    }

    bool cargarListaMusica(const std::string& carpeta) {
        listaMusica.clear();
        primeraReproduccion = true;

        try {

            std::string patron = carpeta + "\\*.*";
#ifdef _WIN32 
            struct _finddata_t archivoInfo;
            intptr_t handle = _findfirst(patron.c_str(), &archivoInfo);

            if (handle == -1) {
                return false;
            }

            do {
                if (strcmp(archivoInfo.name, ".") == 0 || strcmp(archivoInfo.name, "..") == 0) {
                    continue;
                }

                if (!(archivoInfo.attrib & _A_SUBDIR)) {
                    std::string nombreArchivo = archivoInfo.name;
                    std::string extension = obtenerExtension(nombreArchivo);

                    if (extension == ".mp3" || extension == ".wav" || extension == ".ogg" ||
                        extension == ".flac" || extension == ".m4a" || extension == ".aac") {
                        std::string rutaCompleta = carpeta + "\\" + nombreArchivo;
                        listaMusica.push_back(rutaCompleta);
                    }
                }
            } while (_findnext(handle, &archivoInfo) == 0);

            _findclose(handle);
#else
            cout << carpeta;
            DIR* dir = opendir((carpeta + "/.").c_str());
            if (dir == nullptr) {
            }else{
                struct dirent* entrada;
                while ((entrada = readdir(dir)) != nullptr) {
                    // Ignorar "." y ".."
                    if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0)
                        continue;

                    listaMusica.push_back(entrada->d_name);
                }
                closedir(dir);
            }
#endif
            if (!listaMusica.empty()) {
                generarPlaylistAleatoria();
            }

            return !listaMusica.empty();
        }
        catch (const std::exception& e) {
            return false;
        }
    }

    bool reproducirSiguienteCancion() {
        if (listaMusica.empty()) {
            return false;
        }

        if (primeraReproduccion || indicePlaylist >= playlistAleatoria.size() - 1) {
            generarPlaylistAleatoria();
            primeraReproduccion = false;
        }

        indicePlaylist++;

        if (musicaStream) {
            BASS_ChannelStop(musicaStream);
            BASS_StreamFree(musicaStream);
            musicaStream = 0;
        }

        int indiceReal = playlistAleatoria[indicePlaylist];

        musicaStream = BASS_StreamCreateFile(FALSE, listaMusica[indiceReal].c_str(), 0, 0, 0);
        if (!musicaStream) {
            return false;
        }

        // Aplicar volumen antes de reproducir
        aplicarVolumenMusica();

        reproduciendo = BASS_ChannelPlay(musicaStream, TRUE);
        return reproduciendo;
    }

    bool reproducirMusicaAleatoria() {
        return reproducirSiguienteCancion();
    }

    bool reproducirCancion(int indice) {
        if (indice < 0 || indice >= listaMusica.size()) {
            return false;
        }

        if (musicaStream) {
            BASS_ChannelStop(musicaStream);
            BASS_StreamFree(musicaStream);
        }

        musicaStream = BASS_StreamCreateFile(FALSE, listaMusica[indice].c_str(), 0, 0, 0);
        if (!musicaStream) {
            return false;
        }

        // Aplicar volumen antes de reproducir
        aplicarVolumenMusica();

        reproduciendo = BASS_ChannelPlay(musicaStream, TRUE);
        return reproduciendo;
    }

    bool siguienteCancion() {
        return reproducirSiguienteCancion();
    }

    bool necesitaSiguienteCancion() const {
        if (!musicaStream) return true;
        DWORD estado = BASS_ChannelIsActive(musicaStream);
        return (estado == BASS_ACTIVE_STOPPED);
    }

    bool estaReproduciendo() const {
        if (!musicaStream) return false;
        DWORD estado = BASS_ChannelIsActive(musicaStream);
        return (estado == BASS_ACTIVE_PLAYING);
    }

    std::string getCancionActual() const {
        if (indicePlaylist >= 0 && indicePlaylist < playlistAleatoria.size()) {
            int indiceReal = playlistAleatoria[indicePlaylist];
            return obtenerNombreArchivo(listaMusica[indiceReal]);
        }
        return "";
    }

    std::string getProgresoPlaylist() const {
        if (playlistAleatoria.empty()) return "0/0";
        return std::to_string(indicePlaylist + 1) + "/" + std::to_string(playlistAleatoria.size());
    }

    size_t getTotalCanciones() const {
        return listaMusica.size();
    }

    void pausarMusica() {
        if (musicaStream && reproduciendo) {
            BASS_ChannelPause(musicaStream);
            reproduciendo = false;
        }
    }

    void reanudarMusica() {
        if (musicaStream && !reproduciendo) {
            BASS_ChannelPlay(musicaStream, FALSE);
            reproduciendo = true;
        }
    }

    void detenerMusica() {
        if (musicaStream) {
            BASS_ChannelStop(musicaStream);
            reproduciendo = false;
        }
    }

    // ========== CONTROL DE VOLUMEN ==========

    // Establecer volumen de m�sica (0-100)
    void setVolumen(int volumen) {
        // Limitar entre 0 y 100
        if (volumen < 0) volumen = 0;
        if (volumen > 100) volumen = 100;

        // Convertir de 0-100 a 0.0-1.0
        volumenMusica = volumen / 100.0f;

        // Aplicar al stream actual si existe
        aplicarVolumenMusica();
    }

    // Obtener volumen actual (0-100)
    int getVolumen() const {
        return static_cast<int>(volumenMusica * 100);
    }

    // Establecer volumen con float directamente (0.0-1.0)
    void setVolumenFloat(float volumen) {
        if (volumen < 0.0f) volumen = 0.0f;
        if (volumen > 1.0f) volumen = 1.0f;

        volumenMusica = volumen;
        aplicarVolumenMusica();
    }

    // Obtener volumen como float (0.0-1.0)
    float getVolumenFloat() const {
        return volumenMusica;
    }

    // Subir volumen
    void subirVolumen(int cantidad = 5) {
        setVolumen(getVolumen() + cantidad);
    }

    // Bajar volumen
    void bajarVolumen(int cantidad = 5) {
        setVolumen(getVolumen() - cantidad);
    }

    // Silenciar/Activar sonido
    void mutear() {
        setVolumen(0);
    }

    void desmutear(int volumenDefault = 100) {
        setVolumen(volumenDefault);
    }

    // ========== EFECTOS DE SONIDO ==========

    bool cargarEfecto(const char* archivo) {
        HSAMPLE sample = BASS_SampleLoad(FALSE, archivo, 0, 0, 3, 0);
        if (!sample) return false;

        efectosSamples.push_back(sample);
        return true;
    }

    bool reproducirEfecto(int indiceEfecto) {
        if (indiceEfecto < 0 || indiceEfecto >= efectosSamples.size()) {
            return false;
        }

        HCHANNEL channel = BASS_SampleGetChannel(efectosSamples[indiceEfecto], FALSE);
        if (!channel) return false;

        // Aplicar volumen de efectos
        BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volumenEfectos);

        efectosChannels.push_back(channel);
        return BASS_ChannelPlay(channel, TRUE);
    }

    void pararTodosEfectos() {
        for (HCHANNEL channel : efectosChannels) {
            BASS_ChannelStop(channel);
        }
        efectosChannels.clear();
    }

    // Volumen de efectos (0-100)
    void setVolumenEfectos(int volumen) {
        if (volumen < 0) volumen = 0;
        if (volumen > 100) volumen = 100;

        volumenEfectos = volumen / 100.0f;

        // Aplicar a todos los efectos activos
        for (HCHANNEL channel : efectosChannels) {
            BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volumenEfectos);
        }
    }

    int getVolumenEfectos() const {
        return static_cast<int>(volumenEfectos * 100);
    }

    // ========== DEBUG ==========

    std::string getDebugInfo() const {
        std::string info = "Canciones cargadas: " + std::to_string(listaMusica.size()) +
            "\nPlaylist actual: " + std::to_string(playlistAleatoria.size()) +
            "\n�ndice actual: " + std::to_string(indicePlaylist) +
            "\nReproduciendo: " + std::string(reproduciendo ? "S�" : "No") +
            "\nVolumen m�sica: " + std::to_string(getVolumen()) + "%" +
            "\nVolumen efectos: " + std::to_string(getVolumenEfectos()) + "%";

        if (musicaStream) {
            DWORD estado = BASS_ChannelIsActive(musicaStream);
            std::string estadoStr;
            switch (estado) {
            case BASS_ACTIVE_STOPPED: estadoStr = "Detenido"; break;
            case BASS_ACTIVE_PLAYING: estadoStr = "Reproduciendo"; break;
            case BASS_ACTIVE_STALLED: estadoStr = "Stalled"; break;
            case BASS_ACTIVE_PAUSED: estadoStr = "Pausado"; break;
            default: estadoStr = "Desconocido";
            }
            info += "\nEstado stream: " + estadoStr;
        }

        return info;
    }
};