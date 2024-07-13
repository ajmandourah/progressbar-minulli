#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_rotozoom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PROGRESS_BAR_WIDTH (SCREEN_WIDTH - 100) // 540 pixels wide
#define PROGRESS_BAR_HEIGHT 10
#define PROGRESS_BAR_X 50
#define PROGRESS_BAR_Y (SCREEN_HEIGHT - 60) // 50 px from the bottom

const int MAX_SCRIPT_NUM = 31; // Based on S31emulationstation being the last

typedef struct {
    char *script_name;
    char *description;
} ScriptMapping;

ScriptMapping script_descriptions[] = {
    {"S00bootcustom", "Custom boot-up processes..."},
    {"S01date", "Setting up system date..."},
    {"S01dbus", "Starting D-Bus system message bus..."},
    {"S01seedrng", "Seeding random number generator..."},
    {"S01syslogd", "Starting system logger..."},
    {"S02klogd", "Starting kernel logger..."},
    {"S02overclock", "Configuring system overclocking..."},
    {"S02resize", "Resizing filesystem..."},
    {"S02sysctl", "Applying system control parameters..."},
    {"S03modules", "Loading kernel modules..."},
    {"S03urandom", "Saving random seed..."},
    {"S04populate", "Populating system files..."},
    {"S05avahi-setup.sh", "Setting up Avahi daemon..."},
    {"S05udev", "Starting udev device manager..."},
    {"S06audio", "Initializing audio system..."},
    {"S06modprobe", "Executing system modprobe..."},
    {"S07network", "Setting up network configurations..."},
    {"S08connman", "Starting network manager..."},
    {"S11share", "Setting up SHARE partition..."},
    {"S12populateshare", "Populating network shares..."},
    {"S13irqbalance", "Balancing IRQs across CPUs..."},
    {"S15virtualevents", "Setting up virtual input devices..."},
    {"S18governor", "Configuring CPU governor..."},
    {"S25lircd", "Starting IR remote daemon..."},
    {"S26system", "Initializing system settings..."},
    {"S27audioconfig", "Configuring audio settings..."},
    {"S27brightness", "Adjusting screen brightness..."},
    {"S30rpcbind", "Starting RPC bind..."},
    {"S31emulationstation", "Launching Emulation Station..."}
};


const char* get_script_basename(const char *script_path) {
    const char *slash = strrchr(script_path, '/');  // Find the last '/' character
    if (slash) {
        return slash + 1;  // Return the substring after the last '/'
    }
    return script_path;  // Return the original path if no '/' found
}

int get_script_number(const char *script_path) {
    const char *script_name = get_script_basename(script_path);
    int number = 0;
    while (*script_name && !isdigit(*script_name)) ++script_name;
    sscanf(script_name, "%d", &number);
    return number;
}

float calculate_percentage(const char *script_path) {
    int script_number = get_script_number(script_path);
    return script_number * 100.0 / MAX_SCRIPT_NUM;
}

const char* get_description(const char *script_path) {
    const char *script_name = get_script_basename(script_path);
    int num_scripts = sizeof(script_descriptions) / sizeof(script_descriptions[0]);
    for (int i = 0; i < num_scripts; i++) {
        if (strcmp(script_descriptions[i].script_name, script_name) == 0) {
            return script_descriptions[i].description;
        }
    }
    return "Unknown process...";
}

void draw_progress_bar(SDL_Surface *screen, int percentage) {
    SDL_Rect frame = {PROGRESS_BAR_X, PROGRESS_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT};
    SDL_Rect bar = {PROGRESS_BAR_X + 2, PROGRESS_BAR_Y + 2, (PROGRESS_BAR_WIDTH - 4) * percentage / 100, PROGRESS_BAR_HEIGHT - 4};

    // Draw frame
    SDL_FillRect(screen, &frame, SDL_MapRGB(screen->format, 255, 255, 255));
    // Draw bar
    SDL_FillRect(screen, &bar, SDL_MapRGB(screen->format, 0, 128, 0));
}

int main(int argc, char *argv[]) {
/*    if (argc != 3) {
        printf("Usage: %s <percentage> <text>\n", argv[0]);
        return 1;
    }
*/
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Could not initialize SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL) {
        fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load the background image
    SDL_Surface* bgSurface = SDL_LoadBMP("/boot/bootlogo.bmp");
    if (bgSurface == NULL) {
        fprintf(stderr, "Could not load background image: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // We need to rotate the logo for the rg28xx
/*    SDL_Surface* rotatedSurface = rotozoomSurface(bgSurface, -90, 1.0, 1);
    if (rotatedSurface == NULL) {
        fprintf(stderr, "Could not rotate surface: %s\n", SDL_GetError());
        SDL_FreeSurface(bgSurface);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_FreeSurface(bgSurface);
*/
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/dejavu/DejaVuSans-Bold.ttf", 18);
    if (!font) {
        fprintf(stderr, "Failed to open font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Color textColor = {255, 255, 255};
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_location = {50, PROGRESS_BAR_Y - 30, 0, 0};

    SDL_Rect destRect = {0, 0, 0, 0}; // Destination rect
//    SDL_BlitSurface(rotatedSurface, NULL, screen, &destRect);
    SDL_BlitSurface(bgSurface, NULL, screen, &destRect);

    char text[240];
    int percentage;
    char line[256];
    FILE *file;
    int running = 1;

    while (running) {
        file = fopen("/tmp/status.txt", "r");
        if (file == NULL) {
            SDL_Delay(100); // Delay a bit before trying again to avoid tight loop
            continue;
        }

        while (fgets(line, sizeof(line), file)) {
            if (strcmp(line, "QUIT\n") == 0) {
		running = 0;
		break;
            }

            if (sscanf(line, "%239[^\n]", text) == 1) {
		percentage = calculate_percentage(text);
		const char *description = get_description((const char*)text);
		printf("FOUND LINE: %d: %s\n", percentage, description);

	        SDL_Rect destRect = {0, 0, 0, 0}; // Destination rect
	        //SDL_BlitSurface(rotatedSurface, NULL, screen, &destRect);
	        SDL_BlitSurface(bgSurface, NULL, screen, &destRect);

                if (text_surface) SDL_FreeSurface(text_surface);
                text_surface = TTF_RenderText_Solid(font, description, textColor);
                SDL_BlitSurface(text_surface, NULL, screen, &text_location);

                draw_progress_bar(screen, percentage);
                SDL_Flip(screen);
		
		if (percentage >= 100) {
		    running = 0;
		    break;
                }
            }
        }
        fclose(file);
        SDL_Delay(1000); // Add a delay to reduce CPU usage
    }

    if (text_surface) SDL_FreeSurface(text_surface);

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}




