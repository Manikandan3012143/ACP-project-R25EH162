#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define WIDTH 70
#define HEIGHT 22
#define MAX_SHAPES 100

// ANSI Terminal Colors
#define ANSI_RESET          "\x1b[0m"
#define ANSI_BOLD           "\x1b[1m"
#define ANSI_DIM            "\x1b[2m"
#define ANSI_COLOR_CANVAS   "\x1b[32m"  // Green for '*'
#define ANSI_COLOR_BG       "\x1b[90m"  // Dark Gray for '_'
#define ANSI_COLOR_HEADER   "\x1b[95m"  // Bright Magenta
#define ANSI_COLOR_MENU     "\x1b[96m"  // Bright Cyan
#define ANSI_COLOR_ACCENT   "\x1b[93m"  // Bright Yellow
#define ANSI_COLOR_ERROR    "\x1b[91m"  // Bright Red
#define ANSI_COLOR_SUCCESS  "\x1b[92m"  // Bright Green
#define ANSI_CLEAR_SCREEN   "\x1b[2J\x1b[H"

typedef enum {
    SHAPE_LINE = 1,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct {
    int x1, y1;
    int x2, y2;
} LineData;

typedef struct {
    int x, y;
    int width, height;
} RectData;

typedef struct {
    int cx, cy;
    int radius;
} CircleData;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleData;

typedef struct {
    int id;
    ShapeType type;
    union {
        LineData line;
        RectData rect;
        CircleData circle;
        TriangleData triangle;
    } data;
    int is_active;
    char name[32];
} Shape;

// Global State
char canvas[HEIGHT][WIDTH];
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;

// Function Declarations
void clear_canvas();
void draw_pixel(int x, int y);
void draw_line(int x1, int y1, int x2, int y2);
void draw_rectangle(int x, int y, int w, int h);
void draw_circle(int cx, int cy, int r);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void render_all_shapes();
void display_canvas();
void print_header();
void clear_input_buffer();

// Initialize canvas with '_'
void clear_canvas() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Draw a pixel on the canvas with clipping boundary checks
void draw_pixel(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

// Bresenham's Line Algorithm
void draw_line(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        draw_pixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Draw Rectangle outline using 4 line segments
void draw_rectangle(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    draw_line(x, y, x + w - 1, y);                 // Top border
    draw_line(x, y + h - 1, x + w - 1, y + h - 1); // Bottom border
    draw_line(x, y, x, y + h - 1);                 // Left border
    draw_line(x + w - 1, y, x + w - 1, y + h - 1); // Right border
}

// Bresenham's (Midpoint) Circle Algorithm
void draw_circle(int cx, int cy, int r) {
    if (r <= 0) return;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (y >= x) {
        draw_pixel(cx + x, cy + y);
        draw_pixel(cx - x, cy + y);
        draw_pixel(cx + x, cy - y);
        draw_pixel(cx - x, cy - y);
        draw_pixel(cx + y, cy + x);
        draw_pixel(cx - y, cy + x);
        draw_pixel(cx + y, cy - x);
        draw_pixel(cx - y, cy - x);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

// Draw Triangle using 3 line segments
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// Rasterize all active shapes in vector list onto the 2D character canvas
void render_all_shapes() {
    clear_canvas();
    for (int i = 0; i < shape_count; i++) {
        if (!shapes[i].is_active) continue;
        
        switch (shapes[i].type) {
            case SHAPE_LINE:
                draw_line(shapes[i].data.line.x1, shapes[i].data.line.y1,
                          shapes[i].data.line.x2, shapes[i].data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(shapes[i].data.rect.x, shapes[i].data.rect.y,
                               shapes[i].data.rect.width, shapes[i].data.rect.height);
                break;
            case SHAPE_CIRCLE:
                draw_circle(shapes[i].data.circle.cx, shapes[i].data.circle.cy,
                            shapes[i].data.circle.radius);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(shapes[i].data.triangle.x1, shapes[i].data.triangle.y1,
                              shapes[i].data.triangle.x2, shapes[i].data.triangle.y2,
                              shapes[i].data.triangle.x3, shapes[i].data.triangle.y3);
                break;
        }
    }
}

// Display the 2D canvas with elegant border styling and high-contrast color formatting
void display_canvas() {
    // Upper Border
    printf("  " ANSI_COLOR_HEADER "  ");
    for (int x = 0; x < WIDTH; x++) printf("==");
    printf("  \n" ANSI_RESET);

    for (int y = 0; y < HEIGHT; y++) {
        // Left border
        printf("  " ANSI_COLOR_HEADER "||" ANSI_RESET);
        for (int x = 0; x < WIDTH; x++) {
            char pixel = canvas[y][x];
            if (pixel == '*') {
                printf(ANSI_COLOR_CANVAS "**" ANSI_RESET);
            } else {
                printf(ANSI_COLOR_BG "__" ANSI_RESET);
            }
        }
        // Right border
        printf("  " ANSI_COLOR_HEADER "||" ANSI_RESET "\n");
    }

    // Lower Border
    printf("  " ANSI_COLOR_HEADER "  ");
    for (int x = 0; x < WIDTH; x++) printf("==");
    printf("  \n" ANSI_RESET);
}

void print_header() {
    printf(ANSI_COLOR_HEADER ANSI_BOLD);
    printf("=========================================================================\n");
    printf("                    🎨 VECTOR-CLI 2D GRAPHICS EDITOR 🎨                  \n");
    printf("=========================================================================\n");
    printf(ANSI_RESET);
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Helper to list all current active shapes
void list_active_shapes() {
    printf(ANSI_COLOR_ACCENT ANSI_BOLD "\n--- Active Objects in Picture ---\n" ANSI_RESET);
    int active_found = 0;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].is_active) {
            active_found++;
            printf("  ID %d: %s [", shapes[i].id, shapes[i].name);
            switch (shapes[i].type) {
                case SHAPE_LINE:
                    printf("Line from (%d,%d) to (%d,%d)]\n",
                           shapes[i].data.line.x1, shapes[i].data.line.y1,
                           shapes[i].data.line.x2, shapes[i].data.line.y2);
                    break;
                case SHAPE_RECTANGLE:
                    printf("Rectangle at (%d,%d), size %dx%d]\n",
                           shapes[i].data.rect.x, shapes[i].data.rect.y,
                           shapes[i].data.rect.width, shapes[i].data.rect.height);
                    break;
                case SHAPE_CIRCLE:
                    printf("Circle center (%d,%d), radius %d]\n",
                           shapes[i].data.circle.cx, shapes[i].data.circle.cy,
                           shapes[i].data.circle.radius);
                    break;
                case SHAPE_TRIANGLE:
                    printf("Triangle points: (%d,%d), (%d,%d), (%d,%d)]\n",
                           shapes[i].data.triangle.x1, shapes[i].data.triangle.y1,
                           shapes[i].data.triangle.x2, shapes[i].data.triangle.y2,
                           shapes[i].data.triangle.x3, shapes[i].data.triangle.y3);
                    break;
            }
        }
    }
    if (!active_found) {
        printf("  (No objects in picture yet. Select 'Add' to create one!)\n");
    }
}

// UI prompts to add a shape
void add_shape_menu() {
    printf(ANSI_COLOR_MENU "\n--- Add Object ---\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("Select shape type: " ANSI_RESET);
    
    int type;
    if (scanf("%d", &type) != 1 || type < 1 || type > 4) {
        printf(ANSI_COLOR_ERROR "Invalid type selection.\n" ANSI_RESET);
        clear_input_buffer();
        return;
    }

    if (shape_count >= MAX_SHAPES) {
        printf(ANSI_COLOR_ERROR "Canvas capacity full (maximum 100 shapes reached).\n" ANSI_RESET);
        return;
    }

    Shape new_shape;
    new_shape.id = next_id++;
    new_shape.type = (ShapeType)type;
    new_shape.is_active = 1;

    switch (type) {
        case SHAPE_LINE:
            printf("Enter x1 y1 x2 y2 (0-%d for x, 0-%d for y): ", WIDTH - 1, HEIGHT - 1);
            if (scanf("%d %d %d %d", &new_shape.data.line.x1, &new_shape.data.line.y1,
                                     &new_shape.data.line.x2, &new_shape.data.line.y2) != 4) {
                printf(ANSI_COLOR_ERROR "Invalid coordinate input!\n" ANSI_RESET);
                clear_input_buffer();
                return;
            }
            snprintf(new_shape.name, sizeof(new_shape.name), "Line");
            break;

        case SHAPE_RECTANGLE:
            printf("Enter top-left x y, width height: ");
            if (scanf("%d %d %d %d", &new_shape.data.rect.x, &new_shape.data.rect.y,
                                     &new_shape.data.rect.width, &new_shape.data.rect.height) != 4) {
                printf(ANSI_COLOR_ERROR "Invalid input!\n" ANSI_RESET);
                clear_input_buffer();
                return;
            }
            snprintf(new_shape.name, sizeof(new_shape.name), "Rectangle");
            break;

        case SHAPE_CIRCLE:
            printf("Enter center cx cy, radius r: ");
            if (scanf("%d %d %d", &new_shape.data.circle.cx, &new_shape.data.circle.cy,
                                  &new_shape.data.circle.radius) != 3) {
                printf(ANSI_COLOR_ERROR "Invalid input!\n" ANSI_RESET);
                clear_input_buffer();
                return;
            }
            snprintf(new_shape.name, sizeof(new_shape.name), "Circle");
            break;

        case SHAPE_TRIANGLE:
            printf("Enter x1 y1, x2 y2, x3 y3: ");
            if (scanf("%d %d %d %d %d %d", &new_shape.data.triangle.x1, &new_shape.data.triangle.y1,
                                           &new_shape.data.triangle.x2, &new_shape.data.triangle.y2,
                                           &new_shape.data.triangle.x3, &new_shape.data.triangle.y3) != 6) {
                printf(ANSI_COLOR_ERROR "Invalid input!\n" ANSI_RESET);
                clear_input_buffer();
                return;
            }
            snprintf(new_shape.name, sizeof(new_shape.name), "Triangle");
            break;
    }

    shapes[shape_count++] = new_shape;
    printf(ANSI_COLOR_SUCCESS "Successfully added %s with ID %d!\n" ANSI_RESET, new_shape.name, new_shape.id);
    clear_input_buffer();
}

// UI prompts to delete a shape
void delete_shape_menu() {
    list_active_shapes();
    printf(ANSI_COLOR_MENU "\nEnter the ID of the object to DELETE: " ANSI_RESET);
    
    int id;
    if (scanf("%d", &id) != 1) {
        printf(ANSI_COLOR_ERROR "Invalid ID.\n" ANSI_RESET);
        clear_input_buffer();
        return;
    }

    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].is_active && shapes[i].id == id) {
            shapes[i].is_active = 0;
            printf(ANSI_COLOR_SUCCESS "Object ID %d deleted successfully!\n" ANSI_RESET, id);
            clear_input_buffer();
            return;
        }
    }
    printf(ANSI_COLOR_ERROR "Object ID %d not found.\n" ANSI_RESET, id);
    clear_input_buffer();
}

// UI prompts to modify an existing shape
void modify_shape_menu() {
    list_active_shapes();
    printf(ANSI_COLOR_MENU "\nEnter the ID of the object to MODIFY: " ANSI_RESET);
    
    int id;
    if (scanf("%d", &id) != 1) {
        printf(ANSI_COLOR_ERROR "Invalid ID.\n" ANSI_RESET);
        clear_input_buffer();
        return;
    }

    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].is_active && shapes[i].id == id) {
            printf(ANSI_COLOR_SUCCESS "Modifying Object %d (%s):\n" ANSI_RESET, id, shapes[i].name);
            switch (shapes[i].type) {
                case SHAPE_LINE:
                    printf("Current: Line from (%d,%d) to (%d,%d)\n",
                           shapes[i].data.line.x1, shapes[i].data.line.y1,
                           shapes[i].data.line.x2, shapes[i].data.line.y2);
                    printf("Enter NEW x1 y1 x2 y2: ");
                    if (scanf("%d %d %d %d", &shapes[i].data.line.x1, &shapes[i].data.line.y1,
                                             &shapes[i].data.line.x2, &shapes[i].data.line.y2) != 4) {
                        printf(ANSI_COLOR_ERROR "Modification failed: Invalid input.\n" ANSI_RESET);
                    } else {
                        printf(ANSI_COLOR_SUCCESS "Object modified!\n" ANSI_RESET);
                    }
                    break;
                case SHAPE_RECTANGLE:
                    printf("Current: Rectangle at (%d,%d), size %dx%d\n",
                           shapes[i].data.rect.x, shapes[i].data.rect.y,
                           shapes[i].data.rect.width, shapes[i].data.rect.height);
                    printf("Enter NEW top-left x y, width height: ");
                    if (scanf("%d %d %d %d", &shapes[i].data.rect.x, &shapes[i].data.rect.y,
                                             &shapes[i].data.rect.width, &shapes[i].data.rect.height) != 4) {
                        printf(ANSI_COLOR_ERROR "Modification failed: Invalid input.\n" ANSI_RESET);
                    } else {
                        printf(ANSI_COLOR_SUCCESS "Object modified!\n" ANSI_RESET);
                    }
                    break;
                case SHAPE_CIRCLE:
                    printf("Current: Circle center (%d,%d), radius %d\n",
                           shapes[i].data.circle.cx, shapes[i].data.circle.cy,
                           shapes[i].data.circle.radius);
                    printf("Enter NEW center cx cy, radius r: ");
                    if (scanf("%d %d %d", &shapes[i].data.circle.cx, &shapes[i].data.circle.cy,
                                          &shapes[i].data.circle.radius) != 3) {
                        printf(ANSI_COLOR_ERROR "Modification failed: Invalid input.\n" ANSI_RESET);
                    } else {
                        printf(ANSI_COLOR_SUCCESS "Object modified!\n" ANSI_RESET);
                    }
                    break;
                case SHAPE_TRIANGLE:
                    printf("Current: Triangle points: (%d,%d), (%d,%d), (%d,%d)\n",
                           shapes[i].data.triangle.x1, shapes[i].data.triangle.y1,
                           shapes[i].data.triangle.x2, shapes[i].data.triangle.y2,
                           shapes[i].data.triangle.x3, shapes[i].data.triangle.y3);
                    printf("Enter NEW x1 y1, x2 y2, x3 y3: ");
                    if (scanf("%d %d %d %d %d %d", &shapes[i].data.triangle.x1, &shapes[i].data.triangle.y1,
                                                   &shapes[i].data.triangle.x2, &shapes[i].data.triangle.y2,
                                                   &shapes[i].data.triangle.x3, &shapes[i].data.triangle.y3) != 6) {
                        printf(ANSI_COLOR_ERROR "Modification failed: Invalid input.\n" ANSI_RESET);
                    } else {
                        printf(ANSI_COLOR_SUCCESS "Object modified!\n" ANSI_RESET);
                    }
                    break;
            }
            clear_input_buffer();
            return;
        }
    }
    printf(ANSI_COLOR_ERROR "Object ID %d not found.\n" ANSI_RESET, id);
    clear_input_buffer();
}

int main() {
    // Windows Terminal configuration to enable ANSI escape support
    #ifdef _WIN32
    // Force output buffering off to make sure ANSI rendering is direct
    setvbuf(stdout, NULL, _IONBF, 0);
    #endif

    int choice = 0;
    
    // Initial display
    render_all_shapes();

    while (1) {
        printf(ANSI_CLEAR_SCREEN);
        print_header();
        display_canvas();
        
        list_active_shapes();
        
        printf(ANSI_COLOR_MENU ANSI_BOLD "\n=== Menu Actions ===\n" ANSI_RESET);
        printf("1. " ANSI_COLOR_SUCCESS "Add Shape" ANSI_RESET " (Line, Rect, Circle, Triangle)\n");
        printf("2. " ANSI_COLOR_ACCENT "Modify Shape" ANSI_RESET " (Change dimensions/coords)\n");
        printf("3. " ANSI_COLOR_ERROR "Delete Shape" ANSI_RESET " (Remove from picture)\n");
        printf("4. " ANSI_COLOR_HEADER "Clear Canvas" ANSI_RESET " (Delete all shapes)\n");
        printf("5. Refresh Display\n");
        printf("6. Exit Editor\n");
        printf(ANSI_COLOR_MENU "Select Action: " ANSI_RESET);
        
        if (scanf("%d", &choice) != 1) {
            printf(ANSI_COLOR_ERROR "Invalid entry. Please enter a number.\n" ANSI_RESET);
            clear_input_buffer();
            continue;
        }

        switch (choice) {
            case 1:
                add_shape_menu();
                render_all_shapes();
                break;
            case 2:
                modify_shape_menu();
                render_all_shapes();
                break;
            case 3:
                delete_shape_menu();
                render_all_shapes();
                break;
            case 4:
                // Deactivate all
                for (int i = 0; i < shape_count; i++) {
                    shapes[i].is_active = 0;
                }
                shape_count = 0;
                render_all_shapes();
                printf(ANSI_COLOR_SUCCESS "Canvas cleared!\n" ANSI_RESET);
                clear_input_buffer();
                break;
            case 5:
                render_all_shapes();
                clear_input_buffer();
                break;
            case 6:
                printf(ANSI_COLOR_SUCCESS "\nThank you for using Vector-CLI Editor! Goodbye.\n" ANSI_RESET);
                return 0;
            default:
                printf(ANSI_COLOR_ERROR "Invalid choice. Please select 1-6.\n" ANSI_RESET);
                clear_input_buffer();
                break;
        }
        
        printf("\nPress Enter to continue...");
        getchar();
    }

    return 0;
}
