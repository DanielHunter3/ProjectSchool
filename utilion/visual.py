import json
import matplotlib.pyplot as plt
from typing import Dict, Any, List, Optional, Union, Tuple
from dataclasses import dataclass


if __name__ == "__main__":
    raise ImportError("Must be not __main__")


@dataclass
class GraphConfig:
    """Конфигурация графика"""
    title: str
    x_label: str
    y_label: str
    figsize: Tuple[int, int] = (10, 6)


class JsonDataLoader:
    """Загрузчик и парсер JSON данных"""
    
    def __init__(self, filenames: List[str]):
        self._storage: Dict[str, Any] = {}
        self.load_files(filenames)
    
    def load_files(self, filenames: List[str]) -> None:
        """Загружает несколько JSON файлов"""
        for filename in filenames:
            self.load_file(filename)
    
    def load_file(self, filename: str) -> None:
        """Загружает один JSON файл"""
        try:
            with open(filename, 'r', encoding='utf-8') as f:
                self._storage[filename] = json.load(f)
        except FileNotFoundError:
            print(f"Warning: File '{filename}' not found")
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON in '{filename}': {e}")
    
    def query(self, filename: str, path: Optional[List[Union[str, int]]] = None) -> Optional[Any]:
        """Получает данные по пути"""
        if filename not in self._storage:
            raise KeyError(f"Файл '{filename}' не был загружен")
        
        data = self._storage[filename]
        
        if path is None:
            return data
        
        for key in path:
            if isinstance(data, dict) and key in data:
                data = data[key]
            elif isinstance(data, list) and isinstance(key, int) and 0 <= key < len(data):
                data = data[key]
            else:
                return None
        
        return data
    
    def get_times_data(self, filename: str) -> Optional[List[float]]:
        """Специализированный метод для получения массива times"""
        return self.query(filename, ["results", 0, "times"])
    
    @property
    def loaded_files(self) -> List[str]:
        return list(self._storage.keys())


class GraphPlotter:
    """Класс для построения графиков"""
    
    def __init__(self, config: GraphConfig):
        self.config = config
    
    def plot(self, data_points: List[List[float]], labels: List[str]) -> None:
        """
        Рисует несколько графиков на одном рисунке
        
        Args:
            data_points: Список списков с данными для каждого графика
            labels: Подписи для каждого графика
        """
        if len(data_points) != len(labels):
            raise ValueError("Количество наборов данных должно совпадать с количеством меток")
        
        plt.figure(figsize=self.config.figsize)
        
        for i, (points, label) in enumerate(zip(data_points, labels)):
            x = range(len(points))
            plt.plot(x, points, 'o-', linewidth=2, markersize=6, label=label)
        
        plt.title(self.config.title, fontsize=14, pad=20)
        plt.xlabel(self.config.x_label, fontsize=12)
        plt.ylabel(self.config.y_label, fontsize=12)
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
    
    def save(self, data_points: List[List[float]], labels: List[str], filename: str) -> None:
        """Сохраняет график в файл"""
        # Создаем новую фигуру для сохранения
        self.plot(data_points, labels)
        plt.savefig(filename, dpi=300, bbox_inches='tight')
        plt.close()
    
    def show(self, data_points: List[List[float]], labels: List[str]) -> None:
        """Показывает график на экране"""
        self.plot(data_points, labels)
        plt.show()
        plt.close()


class BenchmarkVisualizer:
    """Главный класс для визуализации тестов"""
    
    def __init__(self, configs: List[Tuple[Dict[str, str], GraphConfig]]):
        """
        Args:
            configs: Список конфигураций в формате (translations, graph_config)
        """
        self.configs = configs
        self.data_loader: Optional[JsonDataLoader] = None
    
    def load_data(self) -> None:
        """Загружает все файлы из всех конфигураций"""
        all_files = []
        for translations, _ in self.configs:
            all_files.extend(translations.keys())
        
        self.data_loader = JsonDataLoader(all_files)
    
    def visualize_all(self) -> None:
        """Визуализирует все графики"""
        if self.data_loader is None:
            self.load_data()
        
        for translations, config in self.configs:
            self._visualize_single_config(translations, config)
    
    def _visualize_single_config(
            self, 
            translations: Dict[str, str], 
            config: GraphConfig,
            save_path: Optional[str] = None
        ) -> None:
        """Визуализирует одну группу графиков"""
        data_points = []
        labels = []
        
        for filepath, label in translations.items():
            times_data = self.data_loader.get_times_data(filepath)
            if times_data:
                data_points.append(times_data)
                labels.append(label)
            else:
                print(f"Warning: No times data found in {filepath}")
        
        if data_points:
            plotter = GraphPlotter(config)
            if save_path:
                plotter.save(data_points, labels, save_path)
            else:
                plotter.show(data_points, labels)
        else:
            print(f"No data to plot for '{config.title}'")

    def save_plots(self, output_dir: str = "plots", file_format: str = "png") -> None:
        """
        Сохраняет все графики в файлы
        
        Args:
            output_dir: Директория для сохранения графиков
            file_format: Формат файла (png, pdf, svg, jpg)
        """
        import os
        
        if self.data_loader is None:
            self.load_data()
        
        # Создаем директорию, если она не существует
        os.makedirs(output_dir, exist_ok=True)
        
        for translations, config in self.configs:
            # Генерируем имя файла на основе заголовка графика
            safe_title = "".join(
                c if c.isalnum() or c in " _-" else "_" 
                for c in config.title
            ).replace(" ", "_").rstrip("_")
            
            filename = f"{safe_title}.{file_format}"
            filepath = os.path.join(output_dir, filename)
            
            print(f"Saving plot to: {filepath}")
            self._visualize_single_config(translations, config, save_path=filepath)