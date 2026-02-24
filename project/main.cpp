#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <iomanip>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

struct Coin {
    Point center;
    int radius;
    double value;
};

double getGrayStdDev(const Mat& gray) {
    Scalar mean, stddev;
    meanStdDev(gray, mean, stddev);
    return stddev[0];
}

Vec3b getMeanHSV(const Mat& bgrImage, Point center, int radius) {
    Mat mask = Mat::zeros(bgrImage.size(), CV_8UC1);
    circle(mask, center, radius * 0.55, Scalar(255), FILLED); 
    Mat hsv;
    cvtColor(bgrImage, hsv, COLOR_BGR2HSV);
    Scalar meanColor = mean(hsv, mask);
    return Vec3b(meanColor[0], meanColor[1], meanColor[2]);
}

string formatCoinName(double value) {
    if (abs(value - 2.00) < 0.001) return "2 EUR";
    if (abs(value - 1.00) < 0.001) return "1 EUR";
    if (abs(value - 0.50) < 0.001) return "50 cts";
    if (abs(value - 0.20) < 0.001) return "20 cts";
    if (abs(value - 0.10) < 0.001) return "10 cts";
    if (abs(value - 0.05) < 0.001) return "5 cts";
    if (abs(value - 0.02) < 0.001) return "2 cts";
    if (abs(value - 0.01) < 0.001) return "1 ct";
    return "???"; 
}

double identifyCoin(double ratio, Vec3b hsv) {
    int H = hsv[0]; 
    int S = hsv[1]; 
    bool isCopper = (H < 15 || H > 160); 
    bool isBimetal = (S < 120) && !isCopper; 
    bool isGold = !isCopper && !isBimetal; 

    if (isBimetal) return (ratio > 0.93) ? 2.00 : 1.00;
    if (isGold) {
        if (ratio > 0.87) return 0.50;
        if (ratio > 0.77) return 0.20;
        return 0.10;
    }
    if (isCopper) {
        if (ratio > 0.77) return 0.05;
        if (ratio > 0.67) return 0.02;
        return 0.01;
    }
    return 0.0;
}

vector<Vec3f> filterOverlappingCircles(const vector<Vec3f>& circles) {
    vector<Vec3f> filtered;
    for (const auto& current : circles) {
        bool isOverlapTooBig = false;
        for (const auto& kept : filtered) {
            double dist = norm(Point2f(current[0], current[1]) - Point2f(kept[0], kept[1]));
            if (dist < (current[2] + kept[2]) * 0.70) {
                isOverlapTooBig = true;
                break;
            }
        }
        if (!isOverlapTooBig) filtered.push_back(current);
    }
    return filtered;
}

vector<Vec3f> filterSmallParasites(const vector<Vec3f>& circles) {
    if (circles.empty()) return circles;

    float maxR = 0;
    for (const auto& c : circles) {
        if (c[2] > maxR) maxR = c[2];
    }

    vector<Vec3f> filtered;
    for (const auto& c : circles) {
        if (c[2] >= maxR * 0.50) { 
            filtered.push_back(c);
        }
    }
    return filtered;
}

// 1. SEGMENTATION
vector<Vec3f> segment(const string& filename, Mat& src) {
    Mat raw = imread(filename, IMREAD_COLOR);
    if (raw.empty()) return {};

    double scale = 1024.0 / max(raw.cols, raw.rows);
    resize(raw, src, Size(), scale, scale, INTER_AREA);

    Mat gray, blur;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    double stddev = getGrayStdDev(gray);
    int p1, p2, minDist;

    if (stddev > 44.0) {
        // --- TEXTURE FORTE ---
        bilateralFilter(gray, blur, 11, 85, 85);
        p1 = 110;
        p2 = 70;
        minDist = 60;
    } 
    else {
        // --- FOND UNI ---
        Mat temp;
        medianBlur(gray, temp, 5); 
        GaussianBlur(temp, blur, Size(7, 7), 1.5);
        p1 = 95;
        p2 = 42;
        minDist = 45;
    }

    vector<Vec3f> rawCircles;
    HoughCircles(blur, rawCircles, HOUGH_GRADIENT, 1.0, minDist, p1, p2, 20, 350);
    vector<Vec3f> unique = filterOverlappingCircles(rawCircles);
    return filterSmallParasites(unique);}

// 2. CLASSIFICATION
vector<Coin> classify(const Mat& src, const vector<Vec3f>& circles) {
    vector<Coin> classifiedCoins;
    if (circles.empty()) return classifiedCoins;

    double maxRadius = 0;
    for (const auto& c : circles) if (c[2] > maxRadius) maxRadius = c[2];

    for (const auto& c : circles) {
        Coin currentCoin;
        currentCoin.center = Point(cvRound(c[0]), cvRound(c[1]));
        currentCoin.radius = cvRound(c[2]);
        
        if (currentCoin.center.x - currentCoin.radius < 0 || currentCoin.center.y - currentCoin.radius < 0 ||
            currentCoin.center.x + currentCoin.radius >= src.cols || currentCoin.center.y + currentCoin.radius >= src.rows)
            continue;

        double ratio = (double)currentCoin.radius / maxRadius;
        Vec3b hsv = getMeanHSV(src, currentCoin.center, currentCoin.radius);
        currentCoin.value = identifyCoin(ratio, hsv);
        classifiedCoins.push_back(currentCoin);
    }
    return classifiedCoins;
}

// 3. AFFICHAGE
void display(const string& filename, const Mat& src, const vector<Coin>& coins) {
    Mat result = src.clone();
    double totalValue = 0.0;
    string shortName = fs::path(filename).filename().string();

    for (const auto& coin : coins) {
        totalValue += coin.value;
        circle(result, coin.center, coin.radius, Scalar(255, 0, 0), 3);
        string label = formatCoinName(coin.value);
        putText(result, label, Point(coin.center.x - 40, coin.center.y), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 255), 2);
    }

    cout << "   -> " << shortName << " : " << coins.size() << " pieces | Total : " << fixed << setprecision(2) << totalValue << " EUR" << endl;
    imshow("Resultat", result);
    if (waitKey(0) == 27) exit(0);
}


// 4. PROCESS
void processImage(const string& path) {
    Mat imageSource;
    vector<Vec3f> circles = segment(path, imageSource);
    if (!imageSource.empty()) {
        vector<Coin> coins = classify(imageSource, circles);
        display(path, imageSource, coins);
    }
}

int main() {
    string rootPath = "data/"; 
    if (!fs::exists(rootPath)) { cerr << "Dossier 'data/' introuvable !" << endl; return -1; }

    vector<string> imagePaths;
    for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
        if (fs::is_regular_file(entry)) {
            string ext = entry.path().extension().string();
            transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".jpg" || ext == ".png" || ext == ".jpeg" || ext == ".webp") {
                imagePaths.push_back(entry.path().string());
            }
        }
    }
    sort(imagePaths.begin(), imagePaths.end());

    cout << "--- Traitement de " << imagePaths.size() << " images ---" << endl;
    for (const string& path : imagePaths) {
        processImage(path);
    }
    return 0;
}