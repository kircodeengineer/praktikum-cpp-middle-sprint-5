# Заменено на стабильную LTS
FROM ubuntu:25.10

# Смена зеркала репозитория и обновление кэша
RUN sed -i 's|security.ubuntu.com|archive.ubuntu.com|g' /etc/apt/sources.list && \
    sed -i 's|archive.ubuntu.com|mirror.yandex.ru|g' /etc/apt/sources.list && \
    apt clean && \
    rm -rf /var/lib/apt/lists/* && \
    apt update


# Установка общих инструментов с флагом --fix-missing
RUN apt install -y --fix-missing \
    python3-minimal \
    python3-pip \
    pipx \
    git \
    less \
    vim \
    sudo \
    cmake \
    make \
    g++-15 \
    clang-format \
    clangd-20

# Установка gnuplot с флагом --allow-downgrades для решения конфликтов зависимостей
RUN apt install -y --fix-missing --allow-downgrades gnuplot

# Очистка
RUN apt autoremove -y \
    && apt autoclean \
    && rm -rf /var/lib/apt/lists/* \
    && rm -rf /var/cache/apt/* \
    && rm -rf /tmp/*

# Обновление версий gcc/g++ до последних
RUN ln -fs /usr/bin/g++-15 /usr/bin/g++
RUN ln -fs /usr/bin/gcc-15 /usr/bin/gcc

ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=1000

RUN printf "\n\n\nReceived args:\n\tUSERNAME=$USERNAME\n\tUSER_UID=$USER_UID\n\tUSER_GID=$USER_GID\n"
RUN test -n "$USER_UID" && test -n "$USER_GID" || (printf "\n\n\n\tERROR: USER_UID (equal to '$USER_UID') or USER_GID (equal to '$USER_GID') is not set!\n\n\n" && exit 1)

# Удаление пользователя Ubuntu во избежание проблем с существующими UID/GID
RUN set -e; \
    if id -u ubuntu >/dev/null 2>&1; then \
        echo "Delete 'ubuntu' user from the base image"; \
        userdel -r ubuntu 2>/dev/null || true; \
    fi; \
    if getent group ubuntu >/dev/null 2>&1; then \
        echo "Delete 'ubuntu' group from the base image"; \
        groupdel ubuntu 2>/dev/null || true; \
    fi

# Создание непривилегированного пользователя 'dev' с хостовыми UID/GID
RUN set -e; \
    echo "Create group $USERNAME with GID=$USER_GID"; \
    groupadd --gid $USER_GID $USERNAME; \
    \
    echo "Create user $USERNAME with UID=$USER_UID and GID=$USER_GID"; \
    useradd -ms /bin/bash --uid $USER_UID --gid $USER_GID $USERNAME; \
    \
    # Настройка опции NOPASSWD
    echo "$USERNAME ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/$USERNAME; \
    chmod 0440 /etc/sudoers.d/$USERNAME; \
    \
    echo "User $USERNAME with UID=$USER_UID and GID=$USER_GID was successfully created"

# Переключение на непривилегированного пользователя
USER $USERNAME

RUN git config --global core.editor code

# Установка Conan через pipx
RUN pipx install conan
RUN pipx ensurepath

# Установка переменных окружения для pipx
ENV PATH="$HOME/.local/bin:$PATH"

# Задание команды по умолчанию
CMD ["/bin/bash"]

