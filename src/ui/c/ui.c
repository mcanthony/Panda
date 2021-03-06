#include "SDL.h"
#include "SDL_image.h"
// FIXME rename pandalib.h to panda.h
#include "pandalib.h"
#include "ui.h"

#include <OpenGL/gl.h>

// -------- Initialization, Windows, and Surfaces --------

void pandaUIInit() {
    static Bit initialized = false;
    if (!initialized) {
        SDL_Init(SDL_INIT_VIDEO);
        panda$ui$events$EventType$$classInit();
        panda$ui$events$Key$$classInit();
        initialized = true;
    }
}

panda$ui$events$Event* pandaCreateEvent(SDL_Event* event);

void panda$ui$UIMessageQueue$pumpEvents(panda$ui$UIMessageQueue* self) {
    panda$threads$MessageQueue$LTpanda$ui$events$Event$GT$post_TYPE* post =
            (panda$threads$MessageQueue$LTpanda$ui$events$Event$GT$post_TYPE*) 
                *(&self->cl->vtable + panda$threads$MessageQueue$LTpanda$ui$events$Event$GT$post_INDEX);    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        post((panda$threads$MessageQueue$LTpanda$ui$events$Event$GT*) self, 
                (panda$ui$events$Event*) pandaCreateEvent(&event));
    }
}

void panda$ui$UIMessageQueue$waitForEvent(panda$ui$UIMessageQueue* self) {
    panda$threads$MessageQueue$LTpanda$ui$events$Event$GT$hasMessage_TYPE* hasMessage =
            (panda$threads$MessageQueue$LTpanda$ui$events$Event$GT$hasMessage_TYPE*) 
                *(&self->cl->vtable + panda$threads$MessageQueue$LTpanda$ui$events$Event$GT$hasMessage_INDEX);    
    if (!hasMessage((panda$threads$MessageQueue$LTpanda$ui$events$Event$GT*) self)) {
        panda$threads$MessageQueue$LTpanda$ui$events$Event$GT$post_TYPE* post =
                (panda$threads$MessageQueue$LTpanda$ui$events$Event$GT$post_TYPE*) 
                    *(&self->cl->vtable + panda$threads$MessageQueue$LTpanda$ui$events$Event$GT$post_INDEX);    
        SDL_Event event;
        SDL_WaitEvent(&event);
        post((panda$threads$MessageQueue$LTpanda$ui$events$Event$GT*) self, 
                (panda$ui$events$Event*) pandaCreateEvent(&event));
    }
}

void panda$ui$UIMessageQueue$waitForEvent_Int32(panda$ui$UIMessageQueue* self,
        int timeout) {
    panda$ui$UIMessageQueue$waitForEvent(self);
}

// -------- GL --------

#define ATTRIB_MAX 256
typedef struct NativeGL {
    SDL_GLContext context;
    void* attribs[ATTRIB_MAX];
} NativeGL;

void panda$ui$Window$initialize(panda$ui$Window* self, String* title, Int32 x, 
        Int32 y, Int32 width, Int32 height) {
    pandaUIInit();
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_Window* window = SDL_CreateWindow(pandaGetString(title), x, y, width, 
            height, SDL_WINDOW_OPENGL);
    self->native = window;
    NativeGL* native = (NativeGL*) MALLOC(sizeof(NativeGL));
    native->context = SDL_GL_CreateContext(window);
    self->glContext = pandaNew(panda$gl$GL);
    self->glContext->window = self;
    self->glContext->native = native;
}

void panda$gl$GL$enable(panda$gl$GL* self, UInt32 cap) {
    glEnable(cap);
}

void panda$gl$GL$disable(panda$gl$GL* self, UInt32 cap) {
    glDisable(cap);
}

void panda$gl$GL$clearColor(panda$gl$GL* self, Real32 r, Real32 g, Real32 b, Real32 a) {
    glClearColor(r, g, b, a);
}

void panda$gl$GL$clear(panda$gl$GL* self, UInt32 mask) {
    glClear(mask);
}

void panda$gl$GL$swapBuffers(panda$gl$GL* self) {
    SDL_GL_SwapWindow((SDL_Window*) self->window->native);
}

UInt32 panda$gl$GL$createProgram(panda$gl$GL* self) {
    return glCreateProgram();
}

UInt32 panda$gl$GL$createShader(panda$gl$GL* self, UInt32 type) {
    return glCreateShader(type);
}

void panda$gl$GL$shaderSource(panda$gl$GL* self, UInt32 shader, 
        panda$core$String* source) {
    const char* string = pandaGetString(source);
    int length = source->chars->$length;
    glShaderSource(shader, 1, &string, &length);
}

void panda$gl$GL$compileShader(panda$gl$GL* self, UInt32 shader) {
    glCompileShader(shader);
}

panda$core$String* panda$gl$GL$getShaderInfoLog(panda$gl$GL* self, 
        UInt32 shader) {
    static const int MAX_LENGTH = 10240;
    char buffer[MAX_LENGTH];
    Int32 length;
    glGetShaderInfoLog(shader, MAX_LENGTH, &length, buffer);
    return pandaNewString(buffer, length);
}

Int32 panda$gl$GL$getShaderiv(panda$gl$GL* self, UInt32 shader, UInt32 pname) {
    Int32 result;
    glGetShaderiv(shader, pname, &result);
    return result;
}

void panda$gl$GL$attachShader(panda$gl$GL* self, UInt32 program, 
        UInt32 shader) {
    glAttachShader(program, shader);
}

void panda$gl$GL$bindAttribLocation(panda$gl$GL* self, UInt32 shader, 
        UInt32 index, panda$core$String* name) {
    glBindAttribLocation(shader, index, pandaGetString(name));
}

void panda$gl$GL$linkProgram(panda$gl$GL* self, UInt32 program) {
    glLinkProgram(program);
}

void panda$gl$GL$useProgram(panda$gl$GL* self, UInt32 program) {
    glUseProgram(program);
}

Int32 panda$gl$GL$getProgramiv(panda$gl$GL* self, UInt32 program, UInt32 pname) {
    Int32 result;
    glGetProgramiv(program, pname, &result);
    return result;
}

panda$core$String* panda$gl$GL$getProgramInfoLog(panda$gl$GL* self, 
        UInt32 program) {
    static const int MAX_LENGTH = 10240;
    char buffer[MAX_LENGTH];
    Int32 length;
    glGetProgramInfoLog(program, MAX_LENGTH, &length, buffer);
    return pandaNewString(buffer, length);
}

void panda$gl$GL$viewport(panda$gl$GL* self, Int32 x, Int32 y, UInt32 width,
        UInt32 height) {
    glViewport(x, y, width, height);
}

UInt32 panda$gl$GL$getAttribLocation(panda$gl$GL* self, UInt32 program, panda$core$String* name) {
    return glGetAttribLocation(program, pandaGetString(name));
}

void panda$gl$GL$vertexAttribPointer(panda$gl$GL* self, UInt32 index, 
        Int32 size, UInt32 type, Bit normalized, UInt32 stride, 
        panda$core$Object* array, UInt32 byteOffset) {
    panda$collections$Array$LTpanda$core$Real32$GT* arr = 
            (panda$collections$Array$LTpanda$core$Real32$GT*) array;
    glVertexAttribPointer(index, size, type, normalized, stride, 
            ((char*) arr->contents->contents) + byteOffset);
    ((NativeGL*) self->native)->attribs[index] = array;
}

void panda$gl$GL$enableVertexAttribArray(panda$gl$GL* self, UInt32 index) {
    glEnableVertexAttribArray(index);
}

UInt32 panda$gl$GL$createTexture(panda$gl$GL* self) {
    UInt32 result;
    glGenTextures(1, &result);
    return result;
}

void panda$gl$GL$activeTexture(panda$gl$GL* self, UInt32 target) {
    glActiveTexture(target);
}

void panda$gl$GL$bindTexture(panda$gl$GL* self, UInt32 target, UInt32 texture) {
    glBindTexture(target, texture);
}

void panda$gl$GL$texImage2D_UInt32_Int32_UInt32_Int32_Int32_UInt32_UInt32_Array$LTUInt8$GT(panda$gl$GL* self, UInt32 target, Int32 level, 
            UInt32 format, Int32 width, Int32 height, UInt32 internalFormat, UInt32 type, 
            panda$collections$Array$LTpanda$core$UInt8$GT* data) {
    glTexImage2D(target, level, format, width, height, 0, internalFormat, type, 
            data->contents->contents);
}

void panda$gl$GL$texImage2D_UInt32_Int32_UInt32_Int32_Int32_UInt32_UInt32_ImmutableArray$LTUInt8$GT(panda$gl$GL* self, UInt32 target, Int32 level, 
            UInt32 format, Int32 width, Int32 height, UInt32 internalFormat, UInt32 type, 
            panda$collections$ImmutableArray$LTpanda$core$UInt8$GT* data) {
    glTexImage2D(target, level, format, width, height, 0, internalFormat, type, 
            data->contents->contents);
}

void panda$gl$GL$texImage2D_UInt32_Int32_UInt32_Int32_Int32_UInt32_UInt32_Array$LTInt8$GT(panda$gl$GL* self, UInt32 target, Int32 level, 
            UInt32 format, Int32 width, Int32 height, UInt32 internalFormat, UInt32 type, 
            panda$collections$Array$LTpanda$core$Int8$GT* data) {
    glTexImage2D(target, level, format, width, height, 0, internalFormat, type, 
            data->contents->contents);
}

void panda$gl$GL$texImage2D_UInt32_Int32_UInt32_Int32_Int32_UInt32_UInt32_ImmutableArray$LTInt8$GT(panda$gl$GL* self, UInt32 target, Int32 level, 
            UInt32 format, Int32 width, Int32 height, UInt32 internalFormat, UInt32 type, 
            panda$collections$ImmutableArray$LTpanda$core$Int8$GT* data) {
    glTexImage2D(target, level, format, width, height, 0, internalFormat, type, 
            data->contents->contents);
}

void panda$gl$GL$texParameter_UInt32_UInt32_Int32(panda$gl$GL* self, 
            UInt32 target, UInt32 pname, Int32 param) {
    glTexParameteri(target, pname, param);
}

void panda$gl$GL$texParameter_UInt32_UInt32_Real32(panda$gl$GL* self, 
            UInt32 target, UInt32 pname, Real32 param) {
    glTexParameterf(target, pname, param);
}

UInt32 panda$gl$GL$getUniformLocation(panda$gl$GL* self, UInt32 program, panda$core$String* name) {
    return glGetUniformLocation(program, pandaGetString(name));
}

void panda$gl$GL$uniform_UInt32_Int32(panda$gl$GL* self, UInt32 index, Int32 value) {
    glUniform1i(index, value);
}

void panda$gl$GL$uniform_UInt32_Real32(panda$gl$GL* self, UInt32 index, Real32 value) {
    glUniform1f(index, value);
}

void panda$gl$GL$uniform_UInt32_Vector2(panda$gl$GL* self, UInt32 index, panda$gl$Vector2* v) {
    glUniform2f(index, v->x, v->y);
}

void panda$gl$GL$uniform_UInt32_Vector3(panda$gl$GL* self, UInt32 index, panda$gl$Vector3* v) {
    glUniform3f(index, v->x, v->y, v->z);
}

void panda$gl$GL$uniform_UInt32_Vector4(panda$gl$GL* self, UInt32 index, panda$gl$Vector4* v) {
    glUniform4f(index, v->x, v->y, v->z, v->w);
}

void panda$gl$GL$uniform_UInt32_Matrix3(panda$gl$GL* self, UInt32 index, panda$gl$Matrix3* m) {
    glUniformMatrix3fv(index, 1, false, m->contents->contents);
}

void panda$gl$GL$uniform_UInt32_Matrix4(panda$gl$GL* self, UInt32 index, panda$gl$Matrix4* m) {
    glUniformMatrix4fv(index, 1, false, m->contents->contents);
}

void panda$gl$GL$drawArrays(panda$gl$GL* self, UInt32 mode, Int32 first, 
        UInt32 count) {
    glDrawArrays(mode, first, count);
}

void panda$gl$GL$blendFunc(panda$gl$GL* self, UInt32 src, UInt32 dst) {
    glBlendFunc(src, dst);
}

UInt32 panda$gl$GL$getError(panda$gl$GL* self) {
    return glGetError();
}

void panda$ui$Window$close(panda$ui$Window* window) {
    SDL_DestroyWindow(window->native);
}

// -------- Images --------

void pandaPremultiply(UInt8* pixels, int pitch, int width, int height) {
    int x;
    int y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            float alpha = pixels[x * 4 + 3] / 255.0;
            pixels[x * 4] *= alpha;
            pixels[x * 4 + 1] *= alpha;
            pixels[x * 4 + 2] *= alpha;
        }
        pixels += pitch;
    }
}

panda$ui$images$Image* panda$ui$images$Image$load(panda$io$File* file) {
    SDL_Surface* surface = IMG_Load(pandaGetString(file->path));
    panda$ui$images$Image* result = pandaNew(panda$ui$images$Image);
    result->width = surface->w;
    result->height = surface->h;
    panda$collections$PrimitiveArray$LTpanda$core$UInt8$GT* pixels;
    size_t size = result->height * surface->pitch;
    pixels = (panda$collections$PrimitiveArray$LTpanda$core$UInt8$GT*) 
            pandaNewPrimitiveArrayWithLength(&panda$collections$PrimitiveArray$LTpanda$core$UInt8$GT_class, 
                size, 1, false);
    memcpy(pixels->contents, surface->pixels, size);
    result->pixels = pandaNew(panda$collections$ImmutableArray$LTpanda$core$UInt8$GT);
    panda$collections$ImmutableArray$LTpanda$core$UInt8$GT$init_PrimitiveArray$LTpanda$core$UInt8$GT(result->pixels,
            pixels);
    return result;
}

// -------- Event Handling --------

void pandaInitializeKeyEvent(panda$ui$events$KeyEvent* result, 
        SDL_KeyboardEvent* event) {
    switch (event->keysym.sym) {
        case SDLK_a:
            result->key = class_panda$ui$events$Key$A; 
            break;
        case SDLK_b:
            result->key = class_panda$ui$events$Key$B;
            break;
        case SDLK_c:
            result->key = class_panda$ui$events$Key$C;
            break;
        case SDLK_d:
            result->key = class_panda$ui$events$Key$D;
            break;
        case SDLK_e:
            result->key = class_panda$ui$events$Key$E;
            break;
        case SDLK_f:
            result->key = class_panda$ui$events$Key$F;
            break;
        case SDLK_g:
            result->key = class_panda$ui$events$Key$G;
            break;
        case SDLK_h:
            result->key = class_panda$ui$events$Key$H;
            break;
        case SDLK_i:
            result->key = class_panda$ui$events$Key$I;
            break;
        case SDLK_j:
            result->key = class_panda$ui$events$Key$J;
            break;
        case SDLK_k:
            result->key = class_panda$ui$events$Key$K;
            break;
        case SDLK_l:
            result->key = class_panda$ui$events$Key$L;
            break;
        case SDLK_o:
            result->key = class_panda$ui$events$Key$O;
            break;
        case SDLK_p:
            result->key = class_panda$ui$events$Key$P;
            break;
        case SDLK_q:
            result->key = class_panda$ui$events$Key$Q;
            break;
        case SDLK_r:
            result->key = class_panda$ui$events$Key$R;
            break;
        case SDLK_s:
            result->key = class_panda$ui$events$Key$S;
            break;
        case SDLK_t:
            result->key = class_panda$ui$events$Key$T;
            break;
        case SDLK_u:
            result->key = class_panda$ui$events$Key$U;
            break;
        case SDLK_v:
            result->key = class_panda$ui$events$Key$V;
            break;
        case SDLK_w:
            result->key = class_panda$ui$events$Key$W;
            break;
        case SDLK_x:
            result->key = class_panda$ui$events$Key$X;
            break;
        case SDLK_y:
            result->key = class_panda$ui$events$Key$Y;
            break;
        case SDLK_z:
            result->key = class_panda$ui$events$Key$Z;
            break;
        case SDLK_LEFTBRACKET:
            result->key = class_panda$ui$events$Key$LEFT_BRACKET;
            break;
        case SDLK_RIGHTBRACKET:
            result->key = class_panda$ui$events$Key$RIGHT_BRACKET;
            break;
        case SDLK_BACKSPACE: 
            result->key = class_panda$ui$events$Key$BACKSPACE; 
            break;
        case SDLK_TAB: 
            result->key = class_panda$ui$events$Key$TAB; 
            break;
        case SDLK_LEFT: 
            result->key = class_panda$ui$events$Key$LEFT; 
            break;
        case SDLK_RIGHT: 
            result->key = class_panda$ui$events$Key$RIGHT; 
            break;
        case SDLK_UP: 
            result->key = class_panda$ui$events$Key$UP; 
            break;
        case SDLK_DOWN: 
            result->key = class_panda$ui$events$Key$DOWN; 
            break;
        case SDLK_SPACE: 
            result->key = class_panda$ui$events$Key$SPACE; 
            break;
        case SDLK_ESCAPE: 
            result->key = class_panda$ui$events$Key$ESCAPE; 
            break;
        case SDLK_LSHIFT:
            result->key = class_panda$ui$events$Key$LEFT_SHIFT;
            break;
        case SDLK_RSHIFT:
            result->key = class_panda$ui$events$Key$RIGHT_SHIFT;
            break;
        default: 
            result->key = class_panda$ui$events$Key$UNKNOWN; 
    }
    result->modifiers = event->keysym.mod;
}

void pandaInitializeMouseEvent(panda$ui$events$MouseEvent* result,
        SDL_MouseButtonEvent* event) {
    result->button = event->button;
    result->x = event->x;
    result->y = event->y;
}

void pandaInitializeMouseMotionEvent(panda$ui$events$MouseMotionEvent* result,
        SDL_MouseMotionEvent* event) {
    result->x = event->x;
    result->y = event->y;
}

panda$ui$events$Event* pandaCreateEvent(SDL_Event* event) {
    switch (event->type) {
        case SDL_KEYDOWN: {
            panda$ui$events$KeyEvent* result = pandaNew(
                    panda$ui$events$KeyEvent);
            SDL_KeyboardEvent* keyEvent = (SDL_KeyboardEvent*) event;
            result->type = keyEvent->repeat ? 
                class_panda$ui$events$EventType$KEYREPEAT : 
                class_panda$ui$events$EventType$KEYDOWN;
            pandaInitializeKeyEvent(result, keyEvent);
            return (panda$ui$events$Event*) result;
        }
        case SDL_KEYUP: {
            panda$ui$events$KeyEvent* result = pandaNew(
                    panda$ui$events$KeyEvent);
            result->type = class_panda$ui$events$EventType$KEYUP;
            pandaInitializeKeyEvent(result, (SDL_KeyboardEvent*) event);
            return (panda$ui$events$Event*) result;
        }
        case SDL_MOUSEMOTION: {
            panda$ui$events$MouseMotionEvent* result = pandaNew(
                    panda$ui$events$MouseMotionEvent);
            result->type = class_panda$ui$events$EventType$MOUSEMOTION;
            pandaInitializeMouseMotionEvent(result, 
                    (SDL_MouseMotionEvent*) event);
            return (panda$ui$events$Event*) result;
        }
        case SDL_MOUSEBUTTONDOWN: {
            panda$ui$events$MouseEvent* result = pandaNew(
                    panda$ui$events$MouseEvent);
            result->type = class_panda$ui$events$EventType$MOUSEDOWN;
            pandaInitializeMouseEvent(result, 
                    (SDL_MouseButtonEvent*) event);
            return (panda$ui$events$Event*) result;
        }
        case SDL_MOUSEBUTTONUP: {
            panda$ui$events$MouseEvent* result = pandaNew(
                    panda$ui$events$MouseEvent);
            result->type = class_panda$ui$events$EventType$MOUSEUP;
            pandaInitializeMouseEvent(result, 
                    (SDL_MouseButtonEvent*) event);
            return (panda$ui$events$Event*) result;
        }
        default: {
            panda$ui$events$Event* result = pandaNew(
                    panda$ui$events$Event);
            result->type = class_panda$ui$events$EventType$UNKNOWN;
            return result;
        };
    }
}