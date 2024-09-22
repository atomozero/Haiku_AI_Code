#include <ScreenSaver.h>
#include <View.h>
#include <String.h>
#include <cstdlib>
#include <ctime>
#include <vector>

const BString CHARACTERS = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()";
const char* GENIO_MESSAGE = "Genio il miglior IDE per Haiku";

class MatrixScreensaver : public BScreenSaver {
public:
    MatrixScreensaver(BMessage* archive, image_id id)
        : BScreenSaver(archive, id), width(0), height(0), cellSize(24), frameDelay(30000), messageY(0) {
        srand(time(NULL));
    }

    void StartConfig(BView* view) override {
        // Se volessi aggiungere una schermata di configurazione, lo faresti qui
    }

    status_t StartSaver(BView* view, bool preview) override {
        view->SetViewColor(0, 0, 0);
        return B_OK;
    }

    void Draw(BView* view, int32 frame) override {
        BRect screenBounds = view->Bounds();
        width = static_cast<int>(screenBounds.Width() / cellSize);
        height = static_cast<int>(screenBounds.Height() / cellSize);

        if (matrix.empty()) {
            matrix.resize(width);
            speed.resize(width);
            for (int i = 0; i < width; ++i) {
                matrix[i] = rand() % height;
                speed[i] = 1 + rand() % 3;  // Velocità variabile per ogni colonna
            }
        }

        // Usare la modalità drawing ottimizzata per le parti che cambiano
        view->SetDrawingMode(B_OP_ALPHA);
        view->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_COMPOSITE);

        view->SetHighColor(0, 0, 0, 50);  // Trasparenza più leggera per la scia
        view->FillRect(view->Bounds());

        BFont font;
        font.SetSize(cellSize);
        view->SetFont(&font);

        // Disegna la pioggia di caratteri verde
        for (int x = 0; x < width; ++x) {
            int y = matrix[x];
            int distFromTop = 0;

            // Disegna il carattere corrente e la sua scia
            for (int i = 0; i < 20 && y - i >= 0; ++i) {
                distFromTop = i;
                int brightness = 255 - (distFromTop * 12);  // Maggiore gradazione di sfumatura
                view->SetHighColor(0, brightness, 0);  // Colore sfumato

                char c = CHARACTERS[rand() % CHARACTERS.Length()];
                view->DrawString(&c, BPoint(x * cellSize, (y - i) * cellSize));
            }
        }

        // Aggiorna la posizione delle colonne in modo più fluido
        for (int i = 0; i < width; ++i) {
            if (rand() % 10 < 2) {
                matrix[i] = (matrix[i] + speed[i]) % height;
            }
        }

        // Disegna la frase "Genio il miglior IDE per Haiku" al centro, in blu, che scorre verticalmente
        BFont messageFont;
        messageFont.SetSize(cellSize);  // Usa la stessa dimensione dei caratteri della pioggia
        view->SetFont(&messageFont);

        float messageWidth = view->StringWidth(GENIO_MESSAGE);
        float messageX = (screenBounds.Width() - messageWidth) / 2;

        view->SetHighColor(0, 0, 255);  // Colore blu per il testo
        view->DrawString(GENIO_MESSAGE, BPoint(messageX, messageY));

        // Scorri la frase verso il basso
        messageY += cellSize / 2;  // Velocità di scorrimento
        if (messageY > screenBounds.Height()) {
            messageY = 0;  // Resetta la posizione in alto una volta che esce dallo schermo
        }

        snooze(frameDelay);  // Mantiene il ritardo di 30ms
    }

private:
    std::vector<int> matrix;
    std::vector<int> speed;  // Velocità variabile per ogni colonna
    int width;
    int height;
    const int cellSize;
    bigtime_t frameDelay;  // Ritardo del frame ridotto a 30ms
    float messageY;  // Posizione verticale del messaggio
};

extern "C" BScreenSaver* instantiate_screen_saver(BMessage* msg, image_id id) {
    return new MatrixScreensaver(msg, id);
}
