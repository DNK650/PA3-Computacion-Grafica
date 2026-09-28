#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

const float PI = 3.14159265358979323846f;

// ---------------------------
// ROBOT: transformaciones
// ---------------------------
float anguloBase = 0.0f;
float anguloHombro = -25.0f;
float anguloCodo = 45.0f;
float anguloMuneca = 0.0f;
float aperturaPinza = 0.40f;
float escalaRobot = 1.0f;

// ---------------------------
// OBJETO MOVIL DE LA CINTA
// ---------------------------
float cajaX = 0.0f;
bool animarCinta = false;

// ---------------------------
// CAMARA
// ---------------------------
int vistaActual = 0; // 0 perspectiva, 1 frontal, 2 lateral, 3 superior
float camaraAzimut = 45.0f;
float camaraElevacion = 25.0f;
float camaraDistancia = 16.0f;

bool wireframeGlobal = false;

// ---------------------------
// PA3: RENDERIZADO
// ---------------------------
bool texturaPisoActiva = true;
bool luzPuntualActiva = true;
bool posicionLuzAlterna = false;
GLuint texturaPiso = 0;

// ============================================================
// MATERIALES, TEXTURA E ILUMINACION (PA3)
// ============================================================

void materialMate()
{
    const GLfloat especular[] = {0.08f, 0.08f, 0.08f, 1.0f};
    const GLfloat emision[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8.0f);
}

void materialMetalico()
{
    const GLfloat especular[] = {0.80f, 0.80f, 0.82f, 1.0f};
    const GLfloat emision[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 72.0f);
}

void materialPlastico()
{
    const GLfloat especular[] = {0.35f, 0.35f, 0.35f, 1.0f};
    const GLfloat emision[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
}

void crearTexturaPiso()
{
    const int N = 64;
    static unsigned char datos[N * N * 3];

    for (int y = 0; y < N; ++y)
    {
        for (int x = 0; x < N; ++x)
        {
            bool claro = ((x / 8) + (y / 8)) % 2 == 0;
            unsigned char valor = claro ? 175 : 120;
            int i = (y * N + x) * 3;
            datos[i + 0] = valor;
            datos[i + 1] = valor;
            datos[i + 2] = valor;
        }
    }

    glGenTextures(1, &texturaPiso);
    glBindTexture(GL_TEXTURE_2D, texturaPiso);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, N, N, 0,
        GL_RGB, GL_UNSIGNED_BYTE, datos
    );
    glBindTexture(GL_TEXTURE_2D, 0);
}

void configurarIluminacion()
{
    // Condicion 1: iluminacion ambiental global.
    const GLfloat ambienteGlobal[] = {0.22f, 0.22f, 0.24f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambienteGlobal);

    // Condicion 2: luz puntual ubicada junto a la lampara industrial.
    const GLfloat luz0Amb[]  = {0.06f, 0.05f, 0.04f, 1.0f};
    const GLfloat luz0Dif[]  = {0.95f, 0.84f, 0.66f, 1.0f};
    const GLfloat luz0Esp[]  = {1.00f, 0.95f, 0.82f, 1.0f};
    const GLfloat posNormal[] = {-5.75f, 4.45f, -4.80f, 1.0f};
    const GLfloat posAlterna[] = {2.50f, 6.00f, 3.00f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, luz0Amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz0Dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz0Esp);
    glLightfv(GL_LIGHT0, GL_POSITION, posicionLuzAlterna ? posAlterna : posNormal);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.045f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.006f);

    if (luzPuntualActiva)
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);

    // Luz direccional suave de relleno para apreciar volumen.
    const GLfloat luz1Dif[] = {0.34f, 0.40f, 0.52f, 1.0f};
    const GLfloat luz1Esp[] = {0.25f, 0.30f, 0.38f, 1.0f};
    const GLfloat dir[] = {0.40f, 1.00f, 0.25f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luz1Dif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, luz1Esp);
    glLightfv(GL_LIGHT1, GL_POSITION, dir);
    glEnable(GL_LIGHT1);
}

// ============================================================
// UTILIDADES
// ============================================================

void limitarAngulos()
{
    if (anguloBase > 360.0f) anguloBase -= 360.0f;
    if (anguloBase < -360.0f) anguloBase += 360.0f;

    if (anguloHombro > 80.0f) anguloHombro = 80.0f;
    if (anguloHombro < -80.0f) anguloHombro = -80.0f;

    if (anguloCodo > 130.0f) anguloCodo = 130.0f;
    if (anguloCodo < -130.0f) anguloCodo = -130.0f;

    if (anguloMuneca > 180.0f) anguloMuneca = 180.0f;
    if (anguloMuneca < -180.0f) anguloMuneca = -180.0f;

    if (aperturaPinza > 0.75f) aperturaPinza = 0.75f;
    if (aperturaPinza < 0.15f) aperturaPinza = 0.15f;

    if (escalaRobot > 1.5f) escalaRobot = 1.5f;
    if (escalaRobot < 0.6f) escalaRobot = 0.6f;
}

void dibujarPrisma(
    float ancho, float alto, float profundidad,
    float r, float g, float b)
{
    glPushMatrix();
    glColor3f(r, g, b);
    glScalef(ancho, alto, profundidad);
    glutSolidCube(1.0);
    glPopMatrix();
}

void dibujarCilindroY(
    float radio, float alto,
    float r, float g, float b,
    int segmentos = 24)
{
    glPushMatrix();
    glColor3f(r, g, b);

    // glutSolidCylinder crece sobre Z.
    // Rotamos -90 grados en X para hacerlo crecer sobre Y.
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCylinder(radio, alto, segmentos, 8);

    glPopMatrix();
}

void dibujarCilindroX(
    float radio, float largo,
    float r, float g, float b,
    int segmentos = 24)
{
    glPushMatrix();
    glColor3f(r, g, b);

    // Z -> X
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glutSolidCylinder(radio, largo, segmentos, 8);

    glPopMatrix();
}

void dibujarEsfera(
    float radio,
    float r, float g, float b,
    int segmentos = 24)
{
    glColor3f(r, g, b);
    glutSolidSphere(radio, segmentos, segmentos);
}

// ============================================================
// EJES XYZ
// ============================================================

void dibujarEjes()
{
    glLineWidth(2.0f);

    glBegin(GL_LINES);

    // X rojo
    glColor3f(1.0f, 0.1f, 0.1f);
    glVertex3f(0.0f, 0.02f, 0.0f);
    glVertex3f(3.0f, 0.02f, 0.0f);

    // Y verde
    glColor3f(0.1f, 1.0f, 0.1f);
    glVertex3f(0.0f, 0.02f, 0.0f);
    glVertex3f(0.0f, 3.0f, 0.0f);

    // Z azul
    glColor3f(0.1f, 0.4f, 1.0f);
    glVertex3f(0.0f, 0.02f, 0.0f);
    glVertex3f(0.0f, 0.02f, 3.0f);

    glEnd();

    glLineWidth(1.0f);
}

// ============================================================
// OBJETO 1: PISO
// ============================================================

void dibujarPiso()
{
    materialMate();
    glColor3f(0.72f, 0.72f, 0.74f);

    if (texturaPisoActiva)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturaPiso);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-10.0f, 0.0f, -8.0f);
    glTexCoord2f(10.0f, 0.0f);  glVertex3f( 10.0f, 0.0f, -8.0f);
    glTexCoord2f(10.0f, 8.0f);  glVertex3f( 10.0f, 0.0f,  8.0f);
    glTexCoord2f(0.0f, 8.0f);   glVertex3f(-10.0f, 0.0f,  8.0f);
    glEnd();

    if (texturaPisoActiva)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    // Cuadricula industrial: se dibuja sin iluminacion para que sea legible.
    glDisable(GL_LIGHTING);
    glColor3f(0.20f, 0.22f, 0.24f);
    glBegin(GL_LINES);

    for (int x = -10; x <= 10; ++x)
    {
        glVertex3f((float)x, 0.005f, -8.0f);
        glVertex3f((float)x, 0.005f,  8.0f);
    }

    for (int z = -8; z <= 8; ++z)
    {
        glVertex3f(-10.0f, 0.005f, (float)z);
        glVertex3f( 10.0f, 0.005f, (float)z);
    }

    glEnd();
    glEnable(GL_LIGHTING);
}

// ============================================================
// OBJETO 2: MESA DE TRABAJO
// ============================================================

void dibujarMesa()
{
    glPushMatrix();
    glTranslatef(-0.5f, 0.0f, 3.8f);

    // Tablero
    glPushMatrix();
    glTranslatef(0.0f, 1.45f, 0.0f);
    dibujarPrisma(3.6f, 0.22f, 2.0f, 0.18f, 0.32f, 0.48f);
    glPopMatrix();

    // Patas
    const float px[4] = {-1.55f, 1.55f, -1.55f, 1.55f};
    const float pz[4] = {-0.75f, -0.75f, 0.75f, 0.75f};

    for (int i = 0; i < 4; ++i)
    {
        glPushMatrix();
        glTranslatef(px[i], 0.70f, pz[i]);
        dibujarPrisma(0.18f, 1.40f, 0.18f, 0.12f, 0.16f, 0.20f);
        glPopMatrix();
    }

    glPopMatrix();
}

// ============================================================
// OBJETO 3: CINTA TRANSPORTADORA
// ============================================================

void dibujarCintaTransportadora()
{
    glPushMatrix();
    glTranslatef(2.3f, 0.0f, -1.2f);

    // Banda superior
    glPushMatrix();
    glTranslatef(0.0f, 1.10f, 0.0f);
    dibujarPrisma(6.5f, 0.18f, 1.8f, 0.10f, 0.11f, 0.13f);
    glPopMatrix();

    // Laterales
    glPushMatrix();
    glTranslatef(0.0f, 1.28f, 0.82f);
    dibujarPrisma(6.5f, 0.18f, 0.12f, 0.65f, 0.68f, 0.72f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 1.28f, -0.82f);
    dibujarPrisma(6.5f, 0.18f, 0.12f, 0.65f, 0.68f, 0.72f);
    glPopMatrix();

    // Rodillos
    for (float x = -2.8f; x <= 2.8f; x += 0.8f)
    {
        glPushMatrix();
        glTranslatef(x, 1.04f, -0.76f);
        dibujarCilindroX(0.11f, 1.52f, 0.55f, 0.58f, 0.62f);
        glPopMatrix();
    }

    // Soportes
    const float patasX[4] = {-2.7f, -0.9f, 0.9f, 2.7f};
    for (int i = 0; i < 4; ++i)
    {
        glPushMatrix();
        glTranslatef(patasX[i], 0.52f, 0.62f);
        dibujarPrisma(0.16f, 1.04f, 0.16f, 0.22f, 0.24f, 0.28f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(patasX[i], 0.52f, -0.62f);
        dibujarPrisma(0.16f, 1.04f, 0.16f, 0.22f, 0.24f, 0.28f);
        glPopMatrix();
    }

    glPopMatrix();
}

// ============================================================
// OBJETOS 4, 5 y 6: CAJAS
// ============================================================

void dibujarCaja(float x, float z, float r, float g, float b)
{
    glPushMatrix();
    glTranslatef(x, 1.52f, z);

    // Caja principal
    dibujarPrisma(0.95f, 0.75f, 0.95f, r, g, b);

    // Franja superior
    glPushMatrix();
    glTranslatef(0.0f, 0.39f, 0.0f);
    dibujarPrisma(0.98f, 0.05f, 0.98f, 0.88f, 0.80f, 0.56f);
    glPopMatrix();

    glPopMatrix();
}

void dibujarCajas()
{
    // Caja controlada/animada
    dibujarCaja(1.0f + cajaX, -1.2f, 0.76f, 0.43f, 0.16f);

    // Caja 2
    dibujarCaja(3.0f, -1.2f, 0.20f, 0.55f, 0.85f);

    // Caja 3 sobre la mesa
    glPushMatrix();
    glTranslatef(-1.1f, 0.34f, 4.0f);
    dibujarCaja(0.0f, 0.0f, 0.30f, 0.72f, 0.36f);
    glPopMatrix();
}

// ============================================================
// OBJETO 7: ROBOT ARTICULADO - MODELADO JERARQUICO
// ============================================================

void dibujarRobot()
{
    glPushMatrix();

    // Posicion del robot en la escena
    glTranslatef(-4.0f, 0.0f, 0.2f);

    // Escala interactiva
    glScalef(escalaRobot, escalaRobot, escalaRobot);

    // --------------------------------------------------------
    // BASE: padre de toda la cadena
    // --------------------------------------------------------
    glRotatef(anguloBase, 0.0f, 1.0f, 0.0f);

    glPushMatrix();
    dibujarCilindroY(1.10f, 0.45f, 0.20f, 0.22f, 0.26f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    dibujarCilindroY(0.72f, 0.45f, 0.86f, 0.43f, 0.06f);
    glPopMatrix();

    // Llegamos al hombro
    glTranslatef(0.0f, 0.92f, 0.0f);

    // Hombro
    dibujarEsfera(0.48f, 0.95f, 0.55f, 0.08f);
    glRotatef(anguloHombro, 0.0f, 0.0f, 1.0f);

    // --------------------------------------------------------
    // BRAZO
    // --------------------------------------------------------
    glPushMatrix();
    glTranslatef(0.0f, 1.35f, 0.0f);
    dibujarPrisma(0.72f, 2.70f, 0.72f, 0.95f, 0.50f, 0.05f);
    glPopMatrix();

    // Llegamos al codo
    glTranslatef(0.0f, 2.70f, 0.0f);

    dibujarEsfera(0.43f, 0.18f, 0.20f, 0.24f);
    glRotatef(anguloCodo, 0.0f, 0.0f, 1.0f);

    // --------------------------------------------------------
    // ANTEBRAZO
    // --------------------------------------------------------
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, 0.0f);
    dibujarPrisma(0.60f, 2.30f, 0.60f, 1.00f, 0.68f, 0.08f);
    glPopMatrix();

    // Llegamos a muneca
    glTranslatef(0.0f, 2.30f, 0.0f);

    dibujarEsfera(0.36f, 0.22f, 0.24f, 0.28f);
    glRotatef(anguloMuneca, 0.0f, 1.0f, 0.0f);

    // --------------------------------------------------------
    // MUNECA + PINZA
    // --------------------------------------------------------
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, 0.0f);
    dibujarPrisma(1.30f, 0.38f, 0.75f, 0.18f, 0.20f, 0.24f);
    glPopMatrix();

    // Dedo izquierdo
    glPushMatrix();
    glTranslatef(-aperturaPinza, 0.78f, 0.0f);
    dibujarPrisma(0.18f, 0.90f, 0.28f, 0.82f, 0.84f, 0.88f);
    glPopMatrix();

    // Dedo derecho
    glPushMatrix();
    glTranslatef(aperturaPinza, 0.78f, 0.0f);
    dibujarPrisma(0.18f, 0.90f, 0.28f, 0.82f, 0.84f, 0.88f);
    glPopMatrix();

    glPopMatrix();
}

// ============================================================
// OBJETO 8: PANEL DE CONTROL
// ============================================================

void dibujarPanelControl()
{
    glPushMatrix();
    glTranslatef(6.5f, 0.0f, 3.7f);

    // Pedestal
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);
    dibujarPrisma(0.35f, 2.0f, 0.35f, 0.22f, 0.24f, 0.28f);
    glPopMatrix();

    // Consola
    glPushMatrix();
    glTranslatef(0.0f, 2.25f, 0.0f);
    glRotatef(-12.0f, 1.0f, 0.0f, 0.0f);
    dibujarPrisma(1.65f, 1.05f, 0.30f, 0.12f, 0.15f, 0.18f);

    // Pantalla
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.17f);
    dibujarPrisma(1.28f, 0.68f, 0.04f, 0.10f, 0.72f, 0.88f);
    glPopMatrix();

    // Boton de emergencia
    glPushMatrix();
    glTranslatef(0.55f, -0.34f, 0.22f);
    dibujarEsfera(0.12f, 0.95f, 0.08f, 0.08f);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
}

// ============================================================
// OBJETO 9: LAMPARA INDUSTRIAL
// ============================================================

void dibujarLampara()
{
    glPushMatrix();
    glTranslatef(-7.0f, 0.0f, -4.8f);

    // Poste
    dibujarCilindroY(0.13f, 4.6f, 0.20f, 0.22f, 0.25f);

    // Brazo horizontal
    glPushMatrix();
    glTranslatef(0.0f, 4.45f, 0.0f);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    dibujarCilindroY(0.10f, 1.3f, 0.20f, 0.22f, 0.25f);
    glPopMatrix();

    // Cabezal con leve emision para representar la fuente puntual.
    glPushMatrix();
    glTranslatef(1.25f, 4.45f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    const GLfloat emisionLuz[] = {0.38f, 0.24f, 0.03f, 1.0f};
    const GLfloat sinEmision[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emisionLuz);
    glColor3f(0.95f, 0.76f, 0.12f);
    glutSolidCone(0.48f, 0.70f, 24, 8);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sinEmision);
    glPopMatrix();

    glPopMatrix();
}

// ============================================================
// OBJETO 10: BARRERA DE SEGURIDAD
// ============================================================

void dibujarBarrera()
{
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, -5.5f);

    // Postes
    for (float x = -7.0f; x <= 7.0f; x += 2.0f)
    {
        glPushMatrix();
        glTranslatef(x, 1.0f, 0.0f);
        dibujarPrisma(0.12f, 2.0f, 0.12f, 0.92f, 0.72f, 0.08f);
        glPopMatrix();
    }

    // Riel superior
    glPushMatrix();
    glTranslatef(0.0f, 1.75f, 0.0f);
    dibujarPrisma(14.2f, 0.10f, 0.10f, 0.92f, 0.72f, 0.08f);
    glPopMatrix();

    // Riel medio
    glPushMatrix();
    glTranslatef(0.0f, 0.95f, 0.0f);
    dibujarPrisma(14.2f, 0.08f, 0.08f, 0.92f, 0.72f, 0.08f);
    glPopMatrix();

    glPopMatrix();
}

// ============================================================
// CURVA PARAMETRICA: BEZIER CUBICA
// Representa un cable de alimentacion/senal.
// B(t) = (1-t)^3 P0 + 3(1-t)^2 t P1 +
//        3(1-t)t^2 P2 + t^3 P3
// ============================================================

void dibujarCurvaBezier()
{
    const float P0[3] = {-5.8f, 0.8f, 1.2f};
    const float P1[3] = {-6.5f, 4.8f, 1.5f};
    const float P2[3] = {-2.0f, 6.3f, 1.2f};
    const float P3[3] = {-1.2f, 3.8f, 0.8f};

    glLineWidth(4.0f);
    glColor3f(0.08f, 0.08f, 0.08f);

    glBegin(GL_LINE_STRIP);

    for (int i = 0; i <= 80; ++i)
    {
        float t = i / 80.0f;
        float u = 1.0f - t;

        float b0 = u * u * u;
        float b1 = 3.0f * u * u * t;
        float b2 = 3.0f * u * t * t;
        float b3 = t * t * t;

        float x = b0 * P0[0] + b1 * P1[0] + b2 * P2[0] + b3 * P3[0];
        float y = b0 * P0[1] + b1 * P1[1] + b2 * P2[1] + b3 * P3[1];
        float z = b0 * P0[2] + b1 * P1[2] + b2 * P2[2] + b3 * P3[2];

        glVertex3f(x, y, z);
    }

    glEnd();
    glLineWidth(1.0f);
}

// ============================================================
// SUPERFICIE PARAMETRICA GENERADA + WIREFRAME
//
// S(u,v):
// x = -2.2 + 4.4u
// y = 0.4 + 2.8v
// z = -3.8 + 0.28 sin(pi*u) sin(pi*v)
//
// Es un panel protector curvado generado matematicamente.
// ============================================================

void puntoSuperficie(float u, float v, float& x, float& y, float& z)
{
    x = -2.2f + 4.4f * u;
    y = 0.4f + 2.8f * v;
    z = -3.8f + 0.28f * std::sin(PI * u) * std::sin(PI * v);
}

void dibujarSuperficieParametrica()
{
    glColor3f(0.15f, 0.85f, 0.95f);
    glLineWidth(1.5f);

    const int NU = 18;
    const int NV = 12;

    // Lineas en direccion U
    for (int j = 0; j <= NV; ++j)
    {
        float v = (float)j / NV;

        glBegin(GL_LINE_STRIP);

        for (int i = 0; i <= NU; ++i)
        {
            float u = (float)i / NU;
            float x, y, z;
            puntoSuperficie(u, v, x, y, z);
            glVertex3f(x, y, z);
        }

        glEnd();
    }

    // Lineas en direccion V
    for (int i = 0; i <= NU; ++i)
    {
        float u = (float)i / NU;

        glBegin(GL_LINE_STRIP);

        for (int j = 0; j <= NV; ++j)
        {
            float v = (float)j / NV;
            float x, y, z;
            puntoSuperficie(u, v, x, y, z);
            glVertex3f(x, y, z);
        }

        glEnd();
    }

    // Soportes del panel
    glPushMatrix();
    glTranslatef(-2.3f, 1.6f, -3.8f);
    dibujarPrisma(0.12f, 3.2f, 0.12f, 0.16f, 0.18f, 0.22f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.3f, 1.6f, -3.8f);
    dibujarPrisma(0.12f, 3.2f, 0.12f, 0.16f, 0.18f, 0.22f);
    glPopMatrix();

    glLineWidth(1.0f);
}

// ============================================================
// SENSOR DE SEGURIDAD EN WIREFRAME
// ============================================================

void dibujarSensor()
{
    glPushMatrix();
    glTranslatef(7.4f, 0.0f, -3.5f);

    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 0.0f);
    dibujarPrisma(0.18f, 2.2f, 0.18f, 0.20f, 0.22f, 0.25f);
    glPopMatrix();

    glTranslatef(0.0f, 2.35f, 0.0f);

    glColor3f(0.95f, 0.18f, 0.15f);
    glutWireSphere(0.42f, 16, 12);

    glPopMatrix();
}

// ============================================================
// CAMARA
// ============================================================

void aplicarCamara()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (vistaActual)
    {
        // Perspectiva orbitable
        case 0:
        {
            float az = camaraAzimut * PI / 180.0f;
            float el = camaraElevacion * PI / 180.0f;

            float x = camaraDistancia * std::cos(el) * std::sin(az);
            float y = 2.0f + camaraDistancia * std::sin(el);
            float z = camaraDistancia * std::cos(el) * std::cos(az);

            gluLookAt(
                x, y, z,
                0.0f, 1.8f, 0.0f,
                0.0f, 1.0f, 0.0f
            );
            break;
        }

        // Frontal
        case 1:
            gluLookAt(
                0.0f, 5.0f, 17.0f,
                0.0f, 1.8f, 0.0f,
                0.0f, 1.0f, 0.0f
            );
            break;

        // Lateral
        case 2:
            gluLookAt(
                17.0f, 5.0f, 0.0f,
                0.0f, 1.8f, 0.0f,
                0.0f, 1.0f, 0.0f
            );
            break;

        // Superior
        case 3:
            gluLookAt(
                0.0f, 19.0f, 0.01f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, -1.0f
            );
            break;
    }
}

// ============================================================
// ESCENA COMPLETA
// ============================================================

void dibujarEscena()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    aplicarCamara();
    configurarIluminacion();

    if (wireframeGlobal)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Piso texturizado y objetos con materiales diferenciados.
    materialMate();
    dibujarPiso();

    glDisable(GL_LIGHTING);
    dibujarEjes();
    glEnable(GL_LIGHTING);

    materialMetalico();
    dibujarMesa();
    dibujarCintaTransportadora();

    materialMate();
    dibujarCajas();

    materialMetalico();
    dibujarRobot();

    materialPlastico();
    dibujarPanelControl();

    materialMetalico();
    dibujarLampara();
    dibujarBarrera();

    // Elementos lineales del PA2 se mantienen como evidencia de modelado.
    glDisable(GL_LIGHTING);
    dibujarCurvaBezier();
    dibujarSuperficieParametrica();
    glEnable(GL_LIGHTING);

    materialPlastico();
    dibujarSensor();

    glutSwapBuffers();
}

// ============================================================
// PROYECCION / RESIZE
// ============================================================

void redimensionar(int ancho, int alto)
{
    if (alto == 0)
        alto = 1;

    float aspecto = (float)ancho / (float)alto;

    glViewport(0, 0, ancho, alto);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
        60.0,
        aspecto,
        0.1,
        100.0
    );

    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// RESET
// ============================================================

void reiniciar()
{
    anguloBase = 0.0f;
    anguloHombro = -25.0f;
    anguloCodo = 45.0f;
    anguloMuneca = 0.0f;
    aperturaPinza = 0.40f;
    escalaRobot = 1.0f;

    cajaX = 0.0f;
    animarCinta = false;

    vistaActual = 0;
    camaraAzimut = 45.0f;
    camaraElevacion = 25.0f;
    camaraDistancia = 16.0f;

    wireframeGlobal = false;
    texturaPisoActiva = true;
    luzPuntualActiva = true;
    posicionLuzAlterna = false;

    glutPostRedisplay();
}

// ============================================================
// TECLADO
// ============================================================

void teclado(unsigned char tecla, int, int)
{
    switch (tecla)
    {
        // Robot: base
        case '1':
            anguloBase += 5.0f;
            break;

        case '2':
            anguloBase -= 5.0f;
            break;

        // Robot: hombro
        case '3':
            anguloHombro += 5.0f;
            break;

        case '4':
            anguloHombro -= 5.0f;
            break;

        // Robot: codo
        case '5':
            anguloCodo += 5.0f;
            break;

        case '6':
            anguloCodo -= 5.0f;
            break;

        // Muneca
        case '7':
            anguloMuneca += 5.0f;
            break;

        case '8':
            anguloMuneca -= 5.0f;
            break;
            

        // Pinza
        case '9':
            aperturaPinza += 0.05f;
            break;

        case '0':
            aperturaPinza -= 0.05f;
            break;

        // Traslacion de caja
        case 'j':
        case 'J':
            cajaX -= 0.20f;
            break;

        case 'l':
        case 'L':
            cajaX += 0.20f;
            break;

        // Escala robot
        case 'z':
        case 'Z':
            escalaRobot -= 0.05f;
            break;

        case 'x':
        case 'X':
            escalaRobot += 0.05f;
            break;

        // Animacion cinta
        case 'a':
        case 'A':
            animarCinta = !animarCinta;
            break;

        // Wireframe global
        case 'w':
        case 'W':
            wireframeGlobal = !wireframeGlobal;
            break;

        // PA3: activar/desactivar textura del piso
        case 't':
        case 'T':
            texturaPisoActiva = !texturaPisoActiva;
            break;

        // PA3: activar/desactivar luz puntual
        case 'k':
        case 'K':
            luzPuntualActiva = !luzPuntualActiva;
            break;

        // PA3: cambiar posicion de la luz puntual
        case 'p':
        case 'P':
            posicionLuzAlterna = !posicionLuzAlterna;
            break;

        // Reset
        case 'r':
        case 'R':
            reiniciar();
            return;

        // Zoom
        case '+':
        case '=':
            camaraDistancia -= 0.7f;
            if (camaraDistancia < 7.0f)
                camaraDistancia = 7.0f;
            break;

        case '-':
        case '_':
            camaraDistancia += 0.7f;
            if (camaraDistancia > 30.0f)
                camaraDistancia = 30.0f;
            break;

        // ESC
        case 27:
            std::exit(0);
            break;
    }

    limitarAngulos();
    glutPostRedisplay();
}

void tecladoEspecial(int tecla, int, int)
{
    switch (tecla)
    {
        // Vistas requeridas
        case GLUT_KEY_F1:
            vistaActual = 0; // perspectiva
            break;

        case GLUT_KEY_F2:
            vistaActual = 1; // frontal
            break;

        case GLUT_KEY_F3:
            vistaActual = 2; // lateral
            break;

        case GLUT_KEY_F4:
            vistaActual = 3; // superior
            break;

        // Orbita de cámara en vista perspectiva
        case GLUT_KEY_LEFT:
            vistaActual = 0;
            camaraAzimut -= 5.0f;
            break;

        case GLUT_KEY_RIGHT:
            vistaActual = 0;
            camaraAzimut += 5.0f;
            break;

        case GLUT_KEY_UP:
            vistaActual = 0;
            camaraElevacion += 4.0f;
            if (camaraElevacion > 80.0f)
                camaraElevacion = 80.0f;
            break;

        case GLUT_KEY_DOWN:
            vistaActual = 0;
            camaraElevacion -= 4.0f;
            if (camaraElevacion < -10.0f)
                camaraElevacion = -10.0f;
            break;
    }

    glutPostRedisplay();
}

// ============================================================
// TIMER
// ============================================================

void timer(int)
{
    if (animarCinta)
    {
        cajaX += 0.035f;

        if (cajaX > 3.6f)
            cajaX = -2.8f;

        glutPostRedisplay();
    }

    glutTimerFunc(16, timer, 0);
}

// ============================================================
// INICIALIZACION
// ============================================================

void inicializar()
{
    glClearColor(0.08f, 0.11f, 0.16f, 1.0f);

    // Visibilidad / oclusion.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Sombreado suave e iluminacion del pipeline fijo de OpenGL.
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Necesario porque la escena usa muchas escalas no uniformes.
    glEnable(GL_NORMALIZE);

    crearTexturaPiso();
}

void mostrarControles()
{
    std::cout << "\n";
    std::cout << "====================================================\n";
    std::cout << " PA3 - CELDA INDUSTRIAL: RENDERIZADO 3D\n";
    std::cout << "====================================================\n";
    std::cout << " PA3 / PRUEBAS\n";
    std::cout << "  T      -> Textura del piso ON/OFF\n";
    std::cout << "  K      -> Luz puntual ON/OFF\n";
    std::cout << "  P      -> Cambiar posicion de luz puntual\n";
    std::cout << "  F1-F4  -> Comparar posiciones de camara\n";
    std::cout << "\n";
    std::cout << " INTERACCION HEREDADA DEL PA2\n";
    std::cout << "  1/2 3/4 5/6 7/8 -> Articulaciones del robot\n";
    std::cout << "  9/0    -> Abrir/cerrar pinza\n";
    std::cout << "  J/L    -> Mover caja\n";
    std::cout << "  A      -> Animacion de cinta ON/OFF\n";
    std::cout << "  Flechas-> Orbitar camara | +/- Zoom\n";
    std::cout << "  W      -> Wireframe | R Reset | ESC Salir\n";
    std::cout << "====================================================\n";
}

// ============================================================
// MAIN
// ============================================================

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_DOUBLE |
        GLUT_RGB |
        GLUT_DEPTH
    );

    glutInitWindowSize(1100, 720);
    glutInitWindowPosition(100, 60);

    glutCreateWindow(
        "PA3 - Renderizado de Celda Industrial 3D"
    );

    inicializar();
    mostrarControles();

    glutDisplayFunc(dibujarEscena);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(tecladoEspecial);
    glutReshapeFunc(redimensionar);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();

    return 0;
}
