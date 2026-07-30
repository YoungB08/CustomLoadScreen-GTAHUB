#ifndef QT_COMPAT_H
#define QT_COMPAT_H

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;

#define Q_OBJECT
#define signals public
#define slots
#define emit if (true) {} else
#define foreach(variable, container) for (variable : container)

class QObject
{
public:
    explicit QObject(QObject *parent = nullptr)
    {
        setParent(parent);
    }

    virtual ~QObject()
    {
        while (!children_.empty()) {
            QObject *child = children_.back();
            children_.pop_back();
            child->parent_ = nullptr;
            delete child;
        }

        if (parent_ != nullptr)
            parent_->removeChild(this);
    }

    QObject(const QObject&) = delete;
    QObject& operator=(const QObject&) = delete;

    QObject* parent() const
    {
        return parent_;
    }

    void setParent(QObject *parent)
    {
        if (parent_ == parent)
            return;

        if (parent_ != nullptr)
            parent_->removeChild(this);

        parent_ = parent;

        if (parent_ != nullptr)
            parent_->addChild(this);
    }

    template <typename... Args>
    static bool connect(Args&&...)
    {
        return false;
    }

private:
    void addChild(QObject *child)
    {
        if (std::find(children_.begin(), children_.end(), child) == children_.end())
            children_.push_back(child);
    }

    void removeChild(QObject *child)
    {
        auto it = std::find(children_.begin(), children_.end(), child);
        if (it != children_.end())
            children_.erase(it);
    }

    QObject *parent_ = nullptr;
    std::vector<QObject*> children_;
};

class QString
{
public:
    QString() = default;

    QString(const char *value)
        : value_(value != nullptr ? value : "")
    {
    }

    QString(char value)
        : value_(1, value)
    {
    }

    QString(const std::string &value)
        : value_(value)
    {
    }

    QString(std::string &&value)
        : value_(std::move(value))
    {
    }

    bool isEmpty() const
    {
        return value_.empty();
    }

    int length() const
    {
        return static_cast<int>(value_.length());
    }

    int size() const
    {
        return static_cast<int>(value_.size());
    }

    const char* c_str() const
    {
        return value_.c_str();
    }

    std::string toStdString() const
    {
        return value_;
    }

    std::wstring toStdWString() const
    {
        return std::wstring(value_.begin(), value_.end());
    }

    char operator[](size_t index) const
    {
        return value_[index];
    }

    int indexOf(const QString &needle) const
    {
        const size_t pos = value_.find(needle.value_);
        return pos == std::string::npos ? -1 : static_cast<int>(pos);
    }

    QString remove(const QString &needle)
    {
        if (needle.value_.empty())
            return *this;

        size_t pos = 0;
        while ((pos = value_.find(needle.value_, pos)) != std::string::npos)
            value_.erase(pos, needle.value_.length());

        return *this;
    }

    QString& operator+=(const QString &other)
    {
        value_ += other.value_;
        return *this;
    }

    friend QString operator+(const QString &left, const QString &right)
    {
        return QString(left.value_ + right.value_);
    }

    friend QString operator+(const char *left, const QString &right)
    {
        return QString(left) + right;
    }

    friend QString operator+(const QString &left, const char *right)
    {
        return left + QString(right);
    }

    friend bool operator==(const QString &left, const QString &right)
    {
        return left.value_ == right.value_;
    }

    friend bool operator!=(const QString &left, const QString &right)
    {
        return !(left == right);
    }

    friend bool operator<(const QString &left, const QString &right)
    {
        return left.value_ < right.value_;
    }

    template <typename T>
    static QString number(T value)
    {
        std::ostringstream stream;
        stream << std::defaultfloat << value;
        return QString(stream.str());
    }

private:
    std::string value_;
};

class QByteArray
{
public:
    QByteArray() = default;

    QByteArray(const char *data, int size)
        : data_(data, data + size)
    {
    }

    char* data()
    {
        return data_.data();
    }

    const char* data() const
    {
        return data_.data();
    }

    int size() const
    {
        return static_cast<int>(data_.size());
    }

private:
    std::vector<char> data_;
};

template <typename T>
class QVector : public std::vector<T>
{
public:
    using std::vector<T>::vector;

    void push_front(const T &value)
    {
        this->insert(this->begin(), value);
    }

    bool removeOne(const T &value)
    {
        auto it = std::find(this->begin(), this->end(), value);
        if (it == this->end())
            return false;

        this->erase(it);
        return true;
    }
};

template <typename T>
using QList = QVector<T>;

template <typename Key, typename Value>
using QMap = std::map<Key, Value>;

class QRegExp
{
public:
    explicit QRegExp(const char *pattern)
        : expression_(pattern)
    {
    }

    int indexIn(const QString &text)
    {
        subject_ = text.toStdString();
        matched_ = std::regex_search(subject_, match_, expression_);
        return matched_ ? static_cast<int>(match_.position()) : -1;
    }

    QString cap(size_t index) const
    {
        if (!matched_ || index >= match_.size())
            return QString();
        return QString(match_[index].str());
    }

private:
    std::regex expression_;
    std::string subject_;
    std::smatch match_;
    bool matched_ = false;
};

class QFileInfo
{
public:
    explicit QFileInfo(const QString &path)
        : path_(path)
    {
    }

    QString fileName() const
    {
        const std::string path = path_.toStdString();
        const size_t pos = path.find_last_of("\\/");
        if (pos == std::string::npos)
            return path;
        return path.substr(pos + 1);
    }

private:
    QString path_;
};

#endif // QT_COMPAT_H
