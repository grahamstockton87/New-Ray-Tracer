#include <iostream>
#include "vec3.h"
#include "ray.h"
#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include"triangle.h"
#include "camera.h"
#include "sphere.h"
#include "C:\Ray Tracer\New Ray Tracer\New Ray Tracer\SFML-2.5.1-windows-vc15-64-bit\SFML-2.5.1\include\SFML\Graphics.hpp"
#include <string>
#include <future>
#include <algorithm>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const int image_width = 1024;
const auto aspect_ratio = 16.0 / 9.0;
const int image_height = static_cast<int>(image_width / aspect_ratio);

#define CHANNEL_NUM 3

uint8_t* pixels = new uint8_t[image_width * image_height * CHANNEL_NUM];
int index = 0;



using namespace std;

color diffuseColour;
float attenuationd;

pointLight light;


color ray_color(const ray& r, const hittable& world, int depth, int liDepth) {
    hit_record rec;
    ray li;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    
    li = ray(light.position, random_in_unit_sphere());

    if (world.hit(r, rec)) {
        ray scattered;
        
        color attenuation;
        vec3 direction;

        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered, light)) {
            
            return attenuation * ray_color(scattered, world, depth - 1, liDepth);
        }
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.6, 1.0);
}

/*
       direction = rec.p - light.position;
       float distance = direction.length();

       attenuationd = light.exponent * distance * distance +
           light.linear * distance +
           light.constant;

       float diffuseFactor(max(dot(normalize(rec.normal), normalize(direction)), 1.0));

       diffuseColour = vec3(light.col * diffuseFactor);
       vec3 emmison = (diffuseColour / attenuationd);
       */


double focusTriangle(point3 lookfrom, point3 center) {
    double distance = pow(lookfrom.x() - center.x(), 2) + pow(lookfrom.y() - center.y(), 2) + pow(lookfrom.z() - center.z(), 2);
    return sqrt(distance);
}

double focusCircle(point3 lookfrom, vec3 circlePos, double radius) {
    double distance = pow(lookfrom.x() - circlePos.x(), 2) + pow(lookfrom.y() - circlePos.y(), 2) + pow(lookfrom.z() - circlePos.z(), 2);
    return sqrt(distance);
}

vec3 centerOfTriangle(point3 p1, point3 p2, point3 p3) {
    return (p1 + p2 + p3) / 3;
}

    int main() {

        light.col *= 40;

        hittable_list world;
        
        auto material_ground = make_shared<diffuse>(color(1.0, 0.0, 0.0));
        auto metald = make_shared<metal>(color(0.0, 0.8, 0.0));

        vec3 p1(-5, 0, 5);
        vec3 p2(5, 0, 5);
        vec3 p3(-5, 0, -5);
       // world.add(make_shared<triangle>(p1, p2, p3, material_ground));


        p1 = vec3(-3, -1, 7);
        p2 = vec3(0, 1, 5);
        p3 = vec3(1, -1, 5);
        //world.add(make_shared<triangle>(p1, p2, p3, material_ground));

        material_ground = make_shared<diffuse>(color(0.4, 0.4, 0.8));
        auto material_center = make_shared<diffuse>(color(0.7, 0.3, 0.3));
        auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8));
        auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2));

        world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
        world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
        world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
        world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));


        //Camera pos
        point3 lookfrom(0, 0.8, 1);
        point3 lookat(-4, 1, -3);
        vec3 vup(0, 1, 0);

        lookat = point3(0, 0.0, -1);
            //lookat = point3(0.0, 0.5, -1.0);

            //Camera details

        double dist_to_focus = (lookfrom - lookat).length();
        auto aperture = 0.01; //higher makes more defocus
        auto focal_length = 50; //zoom. Higher values zoom out. Lower values zoom in

        //Render Settings
        const int samples_per_pixel = 5;
        //pointLight.col /= samples_per_pixel-1;
        const int max_depth = 5;
        const int max_light_rays_bounces = 5;

        //Image Settings
        auto viewport_height = 2.0;
        auto viewport_width = aspect_ratio * viewport_height;

        auto origin = point3(0, 0, 0);
        auto horizontal = vec3(viewport_width, 0, 0);
        auto vertical = vec3(0, viewport_height, 0);
        auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

        camera cam(lookfrom, lookat, vup, focal_length, aspect_ratio, aperture, dist_to_focus);


        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();


        const char* name = "shadowsda.png";

        // Render

        for (float j = image_height - 1; j >= 0; --j) {
            float percent = 100 * (j / (image_height - 1));
            std::cerr << "\rPercent Rendered: " << static_cast <int>(100 - percent) << "% " << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / (image_width - 1);
                    auto v = (j + random_double()) / (image_height - 1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, world, max_depth, max_light_rays_bounces);
                }

                pixel_color /= samples_per_pixel;

                auto r = pixel_color.x();
                auto g = pixel_color.y();
                auto b = pixel_color.z();

                // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
                if (r != r) r = 0.0;
                if (g != g) g = 0.0;
                if (b != b) b = 0.0;

                auto scale = 1.0;
                r = sqrt(scale * r);
                g = sqrt(scale * g);
                b = sqrt(scale * b);

                int ir = static_cast<int>(256 * clamp(r, 0.0, 0.999));
                int ig = static_cast<int>(256 * clamp(g, 0.0, 0.999));
                int ib = static_cast<int>(256 * clamp(b, 0.0, 0.999));

                pixels[index++] = ir;
                pixels[index++] = ig;
                pixels[index++] = ib;

            }
           
           
        }

        stbi_write_png(name, image_width, image_height, CHANNEL_NUM, pixels, image_width * CHANNEL_NUM);

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();



        std::cout << "\nDone rendering " << name << " in " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) / 1000 << " seconds" << std::endl;
    
        sf::RenderWindow window(sf::VideoMode(800, 700), "SFML works!");

        string path = "E:/Ray Tracer/New Ray Tracer/New Ray Tracer/" + string(name);
        cout << "\n" << path << endl;


        sf::Texture texture;
        if (!texture.loadFromFile("shadowsda.png")) {
            cout << "didnt load" << endl;
        } 

        sf::Sprite sprite(texture);

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();
            window.draw(sprite);
            window.display();
        }

        return 0;




}
    

   